import os
import motmetrics as mm
import numpy as np
import pandas as pd
import sys
print(__file__)#获取当前程序路径，注意：这里打印出来的路径为相对路径
#动态获取绝对路径
print(os.path.abspath(__file__)) #这才是当前程序绝对路径
print(os.path.dirname(os.path.abspath(__file__))) #当前程序上一级目录，其中dirname返回目录名，不要文件名
print(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))#当前程序上上一级目录

BASE_DIR = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.append(BASE_DIR)
from deepsort_utils.evaulation import Evaluator


def _parse_gr(path):
    assert os.path.exists(path)
    assert ".csv" == os.path.splitext(path)[-1]

    print("\nParsing Labels from file \"%s\" to <dict> ..." % path)
    df = pd.read_csv(path, usecols=["frame_idx", "id", "l", "t", "r", "b"])

    cnt_frames = 0
    cnt_objs = 0
    _crt_frame_idx = -1
    _crt_frame_obj_list = []
    res_dict = {}  # { frame_idx: [ (obj_idx, x, y, w, h), ...] }
    for _line_idx in range(len(df)):
        _frame_idx = df["frame_idx"][_line_idx]
        _obj_idx = df["id"][_line_idx]
        _bbox_left, _bbox_top, _bbox_right, _bbox_bottom = \
            df["l"][_line_idx], df["t"][_line_idx], df["r"][_line_idx], df["b"][_line_idx]

        # called once upon entry
        if _crt_frame_idx < 0:
            cnt_frames += 1
            _crt_frame_idx = _frame_idx
            _crt_frame_obj_list = []

        # handles an obj of a new frame
        if _frame_idx != _crt_frame_idx:
            # add to dict
            res_dict[_crt_frame_idx] = _crt_frame_obj_list
            # continue
            _crt_frame_idx = _frame_idx
            _crt_frame_obj_list = []
            # logging
            cnt_frames += 1
            if 0 == (_crt_frame_idx % 200):
                print("%s" % _crt_frame_idx, end=", ")

        # handles an obj
        _obj = (
            # idx
            _obj_idx,
            # x, y
            _bbox_left, _bbox_top
            # width, height
            (-_bbox_left + _bbox_right), (-_bbox_top + _bbox_bottom)
        )
        _crt_frame_obj_list.append(_obj)
    cnt_objs += len(df)
    print("==> Done: %d Objects in %d Frames" % (cnt_objs, cnt_frames))

    return res_dict


def populate_accumulator(gr_path, pred_path):
    gr_dict = _parse_gr(path=gr_path)


if "__main__" == __name__:
    pass
    # populate_accumulator(gr_path="../../data/test_videos/Middle_Camera_9.csv",
    #                      pred_path="../output/results.txt")
