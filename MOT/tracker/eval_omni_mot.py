import os
import numpy as np
from tqdm import tqdm
import pandas as pd
import motmetrics as mm

mm.lap.default_solver = 'lap'
from deepsort_utils.evaluation import Evaluator

TEMP_FN_PREFIX = "__temp_eval_omni_gt"
TEMP_FN_EXT = ".txt"
OUT_DIR = "./output"
USE_CACHE = True  # ALERT!!


def _parse_omni_2_mot(path):
    assert os.path.exists(path)
    assert ".csv" == os.path.splitext(path)[-1]
    _dir = os.path.split(path)[0]
    _fn = os.path.splitext(os.path.split(path)[1])[0]
    temp_fn = os.path.join(_dir, TEMP_FN_PREFIX + _fn + TEMP_FN_EXT)

    if USE_CACHE is True:
        if os.path.exists(temp_fn):
            print("Use CACHED Parse-Ground-Truth \"%s\"" % temp_fn)
            return temp_fn

    # Omni-MOT gt => mot-format gt. see: deepsort_utils/io.write_results()
    save_data = []  # [ (frame_idx, obj_idx, x1, y1, w, h), ... ]
    df = pd.read_csv(path, usecols=["frame_idx", "id", "l", "t", "r", "b"])
    for _line_idx in tqdm(range(len(df))):
        _frame_idx = int(df["frame_idx"][_line_idx])
        _obj_idx = int(df["id"][_line_idx])
        _bbox_left, _bbox_top, _bbox_right, _bbox_bottom = \
            int(df["l"][_line_idx]), int(df["t"][_line_idx]), int(df["r"][_line_idx]), int(df["b"][_line_idx])

        # handles an obj
        _obj = (
            # frame_idx, id
            _frame_idx, _obj_idx,
            # x1, y1
            _bbox_left, _bbox_top,
            # width, height
            (-_bbox_left + _bbox_right), (-_bbox_top + _bbox_bottom)
        )
        save_data.append(_obj)

    save_format = "{frame},{id},{x1},{y1},{w},{h},-1,-1,-1,-1\n"
    with open(temp_fn, 'w') as f:
        for frame_id, track_id, x1, y1, w, h in save_data:
            if track_id < 0:
                continue
            line = save_format.format(frame=frame_id, id=track_id, x1=x1, y1=y1, w=w, h=h)
            f.write(line)
    print("Ground Truth Parsed to \"%s\"" % temp_fn)


def eval_metrics(gt_path, pred_path):
    gt_parsed_path = _parse_omni_2_mot(path=gt_path)
    evaluator = Evaluator(data_root="", seq_name="", data_type="mot", full_path=gt_parsed_path)
    seqs = [""]
    accs = [evaluator.eval_file(pred_path)]
    # get summary
    metrics = mm.metrics.motchallenge_metrics
    mh = mm.metrics.create()
    summary = Evaluator.get_summary(accs, seqs, metrics)
    strsummary = mm.io.render_summary(
        summary,
        formatters=mh.formatters,
        namemap=mm.io.motchallenge_metric_names
    )
    print(strsummary)
    Evaluator.save_summary(summary, os.path.join(OUT_DIR, 'summary_global.xlsx'))


if "__main__" == __name__:
    # _parse_omni_2_mot("../data/test_videos/gt/Middle_Camera_9.csv")
    eval_metrics("../data/test_videos/gt/Middle_Camera_9.csv", "./output/results.txt")
