import os.path
from typing import List
import cv2
import numpy as np
import torch

from .deep.feature_extractor import Extractor  # , FastReIDExtractor
from .sort.nn_matching import NearestNeighborDistanceMetric
from .sort.preprocessing import non_max_suppression
from .sort.detection import Detection
from .sort.tracker import Tracker

__all__ = ['DeepSort']


class DeepSort(object):
    def __init__(self,
                 spectrum_template_dir, spectrum_thresh_dist,  # added
                 model_path, model_config=None,
                 max_dist=0.2, min_confidence=0.3, nms_max_overlap=1.0,
                 max_iou_distance=0.7, max_age=70,
                 n_init=3, nn_budget=100, use_cuda=True):
        self.min_confidence = min_confidence
        self.nms_max_overlap = nms_max_overlap

        if model_config is None:
            self.extractor = Extractor(model_path, use_cuda=use_cuda)
        else:
            self.extractor = None  # FastReIDExtractor(model_config, model_path, use_cuda=use_cuda)

        max_cosine_distance = max_dist
        metric = NearestNeighborDistanceMetric("cosine", max_cosine_distance, nn_budget)
        self.tracker = Tracker(metric, max_iou_distance=max_iou_distance, max_age=max_age, n_init=n_init)

        # added: spectrum template matching & thresholding
        assert os.path.exists(spectrum_template_dir)
        spectrum_templates = []
        for _fn in os.listdir(spectrum_template_dir):
            im = cv2.imread(os.path.join(spectrum_template_dir, _fn))
            im = cv2.cvtColor(im, cv2.COLOR_BGR2RGB)
            spectrum_templates.append(im)
        self.spectrum_templates = spectrum_templates
        self.spectrum_thresh_dist = spectrum_thresh_dist

    def update(self, bbox_xywh, confidences, ori_img, bbox_spectrum_list: List[np.ndarray] = None):
        self.height, self.width = ori_img.shape[:2]
        # generate detections
        if bbox_spectrum_list is None:
            features, valid_box_idx_ref = self._get_features(bbox_xywh, ori_img)
        else:
            features, valid_box_idx_ref = self._get_features_from_spectrum(bbox_spectrum_list)
        # added: apply spectrum template thresholding
        bbox_xywh = bbox_xywh[valid_box_idx_ref]
        confidences = confidences[valid_box_idx_ref]
        bbox_tlwh = self._xywh_to_tlwh(bbox_xywh)
        detections = [Detection(bbox_tlwh[i], conf, features[i])
                      for i, conf in enumerate(confidences) if conf > self.min_confidence]

        # run on non-maximum supression
        boxes = np.array([d.tlwh for d in detections])
        scores = np.array([d.confidence for d in detections])
        indices = non_max_suppression(boxes, self.nms_max_overlap, scores)
        detections = [detections[i] for i in indices]

        # update tracker
        self.tracker.predict()
        self.tracker.update(detections)

        # output bbox identities
        outputs = []
        for track in self.tracker.tracks:
            if not track.is_confirmed() or track.time_since_update > 1:
                continue
            box = track.to_tlwh()
            x1, y1, x2, y2 = self._tlwh_to_xyxy(box)
            track_id = track.track_id
            outputs.append(np.array([x1, y1, x2, y2, track_id], dtype=np.int))
        if len(outputs) > 0:
            outputs = np.stack(outputs, axis=0)
        return outputs

    """
    TODO:
        Convert bbox from xc_yc_w_h to xtl_ytl_w_h
    Thanks JieChen91@github.com for reporting this bug!
    """

    @staticmethod
    def _xywh_to_tlwh(bbox_xywh):
        if isinstance(bbox_xywh, np.ndarray):
            bbox_tlwh = bbox_xywh.copy()
        elif isinstance(bbox_xywh, torch.Tensor):
            bbox_tlwh = bbox_xywh.clone()
        bbox_tlwh[:, 0] = bbox_xywh[:, 0] - bbox_xywh[:, 2] / 2.
        bbox_tlwh[:, 1] = bbox_xywh[:, 1] - bbox_xywh[:, 3] / 2.
        return bbox_tlwh

    def _xywh_to_xyxy(self, bbox_xywh):
        x, y, w, h = bbox_xywh
        x1 = max(int(x - w / 2), 0)
        x2 = min(int(x + w / 2), self.width - 1)
        y1 = max(int(y - h / 2), 0)
        y2 = min(int(y + h / 2), self.height - 1)
        return x1, y1, x2, y2

    def _tlwh_to_xyxy(self, bbox_tlwh):
        """
        TODO:
            Convert bbox from xtl_ytl_w_h to xc_yc_w_h
        Thanks JieChen91@github.com for reporting this bug!
        """
        x, y, w, h = bbox_tlwh
        x1 = max(int(x), 0)
        x2 = min(int(x + w), self.width - 1)
        y1 = max(int(y), 0)
        y2 = min(int(y + h), self.height - 1)
        return x1, y1, x2, y2

    def _xyxy_to_tlwh(self, bbox_xyxy):
        x1, y1, x2, y2 = bbox_xyxy

        t = x1
        l = y1
        w = int(x2 - x1)
        h = int(y2 - y1)
        return t, l, w, h

    def _apply_spectrum_template_matching(self, bbox):
        for _template in self.spectrum_templates:
            _bbox_resize = cv2.resize(bbox, (_template.shape[1], _template.shape[0]))
            _dist = np.linalg.norm(_bbox_resize - _template)
            _dist /= (1. * _template.shape[0] * _template.shape[1] * _template.shape[2])
            # print(_dist)
            if _dist <= self.spectrum_thresh_dist:
                return True
        return False

    def _get_features(self, bbox_xywh, ori_img):
        im_crops = []
        valid_box_ref = []
        for box_idx, box in enumerate(bbox_xywh):
            x1, y1, x2, y2 = self._xywh_to_xyxy(box)
            im = ori_img[y1:y2, x1:x2]
            # added: spectrum template matching
            if self._apply_spectrum_template_matching(im):
                im_crops.append(im)
                valid_box_ref.append(box_idx)
            # im_crops.append(im)
        if im_crops:
            features = self.extractor(im_crops)
        else:
            features = np.array([])
        return features, valid_box_ref

    def _get_features_from_spectrum(self, bbox_spectrum_list: List[np.ndarray]):
        im_crops = []
        valid_box_ref = []
        for box_idx, box_spectrum in enumerate(bbox_spectrum_list):
            # added: spectrum template matching
            if self._apply_spectrum_template_matching(box_spectrum):
                im_crops.append(box_spectrum)
                valid_box_ref.append(box_idx)
        if im_crops:
            features = self.extractor(im_crops)
        else:
            features = np.array([])
        return features, valid_box_ref
