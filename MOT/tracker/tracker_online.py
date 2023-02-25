import os
import cv2
from typing import List
import time
import argparse
import torch
import warnings
import numpy as np
import sys

sys.path.append(os.path.join(os.path.dirname(__file__), 'thirdparty/fast-reid'))

from detector import build_detector
from deep_sort import build_tracker
from deepsort_utils.draw import draw_boxes
from deepsort_utils.parser import get_config
from deepsort_utils.log import get_logger
from deepsort_utils.io import buffer_2_image, write_results

os.chdir("../MOT/tracker")  # path alert


def parse_args():
    parser = argparse.ArgumentParser(prog="online_tracker")
    # parser.add_argument("VIDEO_PATH", type=str)
    parser.add_argument("--config_yolov5", type=str, default="./configs/yolov5.yaml")
    # parser.add_argument("--config_yolov5", type=str, default="../MOT/tracker/configs/yolov5.yaml")  # path alert
    # parser.add_argument("--config_mmdetection", type=str, default="./configs/mmdet.yaml")
    # parser.add_argument("--config_detection", type=str, default="./configs/yolov3.yaml")
    parser.add_argument("--config_deepsort", type=str, default="./configs/deep_sort.yaml")
    # parser.add_argument("--config_deepsort", type=str, default="../MOT/tracker/configs/deep_sort.yaml")  # path alert
    # parser.add_argument("--config_fastreid", type=str, default="./configs/fastreid.yaml")
    # parser.add_argument("--fastreid", action="store_true")
    parser.add_argument("--yolov5", action="store_true", default=True)
    # parser.add_argument("--mmdet", action="store_true")
    # parser.add_argument("--ignore_display", dest="display", action="store_false", default=True)
    parser.add_argument("--display", action="store_true")
    # parser.add_argument("--frame_interval", type=int, default=1)
    parser.add_argument("--display_width", type=int, default=800)
    parser.add_argument("--display_height", type=int, default=600)
    parser.add_argument("--save_path", type=str, default="./output/")
    # parser.add_argument("--save_path", type=str, default="../MOT/tracker/output/")  # path alert
    parser.add_argument("--cpu", dest="use_cuda", action="store_false", default=True)
    # parser.add_argument("--camera", action="store", dest="cam", type=int, default="-1")
    return parser.parse_args()


args = parse_args()
cfg = get_config()
# print(args)
# print(cfg)
if args.yolov5:
    cfg.merge_from_file(args.config_yolov5)
    cfg.USE_YOLOV5 = True
# elif args.mmdet:
#     cfg.merge_from_file(args.config_mmdetection)
#     cfg.USE_MMDET = True
# else:
#     cfg.merge_from_file(args.config_detection)
#     cfg.USE_MMDET = False
cfg.merge_from_file(args.config_deepsort)
# if args.fastreid:
#     cfg.merge_from_file(args.config_fastreid)
#     cfg.USE_FASTREID = True
# else:
#     cfg.USE_FASTREID = False
cfg.USE_FASTREID = False


class TrackerOnline(object):
    def __init__(self):
        self.cfg = cfg
        self.args = args
        self.logger = get_logger("root")

        use_cuda = args.use_cuda and torch.cuda.is_available()
        if not use_cuda:
            warnings.warn("Running in cpu mode which maybe very slow!", UserWarning)

        if args.display:
            cv2.namedWindow("test", cv2.WINDOW_NORMAL)
            cv2.resizeWindow("test", args.display_width, args.display_height)

        self.detector = build_detector(cfg, use_cuda=use_cuda)
        self.deepsort = build_tracker(cfg, use_cuda=use_cuda)
        self.class_names = self.detector.class_names
        self.crt_frame_idx = 0
        # for internal context storage: detection-call -> tracking-call
        self._temp_crt_frame_detect_done = False
        self._temp_crt_frame_img = None
        self._temp_crt_frame_bbox_xywh = None
        self._temp_crt_frame_cls_conf = None
        self._temp_time_elapsed_accum = 0.  # in seconds

    def __enter__(self):

        if self.args.save_path:
            os.makedirs(self.args.save_path, exist_ok=True)

            # path of saved video and results
            self.save_results_path = os.path.join(self.args.save_path, "results__frame=%d.png")

            # logging
            self.logger.info("Save results to {}".format(self.args.save_path))

        return self

    def __exit__(self, exc_type, exc_value, exc_traceback):
        if exc_type:
            print(exc_type, exc_value, exc_traceback)

    def _detect(self, img: np.ndarray) -> List[List[int]]:
        assert self._temp_crt_frame_detect_done is False, "[ERROR] Detection already Applied to the Current Frame."

        self.crt_frame_idx += 1
        time_start = time.time()
        img_rgb = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)

        # do detection
        bbox_xywh, cls_conf, cls_ids = self.detector(img_rgb)

        # # select classes: 2=car, 5=bus, 7=truck
        # mask = (cls_ids == 2) | (cls_ids == 5) | (cls_ids == 7)
        mask = (0 == cls_ids)  # ALERT

        bbox_xywh = bbox_xywh[mask].astype(int)
        # # bbox dilation just in case bbox too small, delete this line if using a better pedestrian detector
        # bbox_xywh[:, 3:] *= 1.2
        cls_conf = cls_conf[mask]

        # save to internal context storage
        time_end = time.time()
        time_elapsed = time_end - time_start
        self._temp_crt_frame_img = img_rgb
        self._temp_crt_frame_bbox_xywh = bbox_xywh
        self._temp_crt_frame_cls_conf = cls_conf
        self._temp_time_elapsed_accum += time_elapsed
        self._temp_crt_frame_detect_done = True

        self.logger.info("time: ({:.03f}+?)s, fps: (...), detection numbers: {}, tracking numbers: (?)" \
                         .format(time_elapsed, bbox_xywh.shape[0]))
        res = bbox_xywh.tolist()
        return res

    def detect(self, img_buffer: bytes) -> List[List[int]]:
        if self._temp_crt_frame_detect_done is True:
            raise AssertionError("Cannot Apply Detection on Detection-Handled Frame Image. Please Call `track()`.")

        img = buffer_2_image(img_buffer=img_buffer, is_debug=False)
        return self._detect(img=img)

    def _track(self):
        pass

    def track(self):
        if self._temp_crt_frame_detect_done is False:
            raise AssertionError("Cannot Apply Tracking on Detection-NOT-Handled Frame Image. Please Call `detect()`.")
        return self._track()

# def run(self):
#     results = []
#     idx_frame = 0
#     while self.vdo.grab():
#         idx_frame += 1
#         if idx_frame % self.args.frame_interval:
#             continue
#
#         start = time.time()
#         _, ori_im = self.vdo.retrieve()
#         im = cv2.cvtColor(ori_im, cv2.COLOR_BGR2RGB)
#
#         # do detection
#         bbox_xywh, cls_conf, cls_ids = self.detector(im)
#
#         # # select classes: 2=car, 5=bus, 7=truck
#         # mask = (cls_ids == 2) | (cls_ids == 5) | (cls_ids == 7)
#         mask = (0 == cls_ids)  # ALERT
#
#         bbox_xywh = bbox_xywh[mask]
#         # # bbox dilation just in case bbox too small, delete this line if using a better pedestrian detector
#         # bbox_xywh[:, 3:] *= 1.2
#         cls_conf = cls_conf[mask]
#
#         # do tracking
#         outputs = self.deepsort.update(bbox_xywh, cls_conf, im)
#
#         # draw boxes for visualization
#         if len(outputs) > 0:
#             bbox_tlwh = []
#             bbox_xyxy = outputs[:, :4]
#             identities = outputs[:, -1]
#             ori_im = draw_boxes(ori_im, bbox_xyxy, identities)
#
#             for bb_xyxy in bbox_xyxy:
#                 bbox_tlwh.append(self.deepsort._xyxy_to_tlwh(bb_xyxy))
#
#             results.append((idx_frame - 1, bbox_tlwh, identities))
#
#         end = time.time()
#
#         if self.args.display:
#             cv2.imshow("test", ori_im)
#             cv2.waitKey(1)
#
#         # if self.args.save_path:
#         #     self.writer.write(ori_im)
#
#
#         # logging
#         self.logger.info("time: {:.03f}s, fps: {:.03f}, detection numbers: {}, tracking numbers: {}" \
#                          .format(end - start, 1 / (end - start), bbox_xywh.shape[0], len(outputs)))
