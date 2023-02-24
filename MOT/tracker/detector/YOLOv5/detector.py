import torch
import logging
import numpy as np
import cv2


class YOLOv5(object):
    def __init__(self, model_path, is_xywh=False, use_cuda=True):
        # net definition
        self.net = torch.hub.load("./detector/YOLOv5", "custom", path=model_path, source="local")
        logger = logging.getLogger("root.detector")
        logger.info('Loading weights from %s... Done!' % (model_path))
        self.device = "cuda" if use_cuda else "cpu"
        self.net.eval()
        self.net.to(self.device)

        # constants
        # self.size = self.net.width, self.net.height
        self.use_cuda = use_cuda
        self.is_xywh = is_xywh
        self.num_classes = len(self.net.names.keys())
        self.class_names = [self.net.names[i] for i in range(self.num_classes)]
        logger.info("YOLOv5 model Initialized.")

    def __call__(self, ori_img):
        # img to tensor
        assert isinstance(ori_img, np.ndarray), "input must be a numpy array!"

        results = self.net(ori_img)
        boxes = results.xyxy[0].cpu()  # tensor (cnt, 6): xmin/ymin/xmax/ymax/confidence/class

        if len(boxes) == 0:
            bbox = torch.FloatTensor([]).reshape([0, 4])
            cls_conf = torch.FloatTensor([])
            cls_ids = torch.LongTensor([])
        else:
            height, width = ori_img.shape[:2]
            bbox = boxes[:, :4]
            if self.is_xywh:
                # bbox x y w h
                bbox = self.xyxy_to_xywh(bbox)

            # bbox *= torch.FloatTensor([[width, height, width, height]])
            cls_conf = boxes[:, 4]
            cls_ids = boxes[:, 5].long()
        return bbox.numpy(), cls_conf.numpy(), cls_ids.numpy()

    @staticmethod
    def xyxy_to_xywh(boxes_xyxy):
        if isinstance(boxes_xyxy, torch.Tensor):
            boxes_xywh = boxes_xyxy.clone()
        elif isinstance(boxes_xyxy, np.ndarray):
            boxes_xywh = boxes_xyxy.copy()

        boxes_xywh[:, 0] = (boxes_xyxy[:, 0] + boxes_xyxy[:, 2]) / 2.
        boxes_xywh[:, 1] = (boxes_xyxy[:, 1] + boxes_xyxy[:, 3]) / 2.
        boxes_xywh[:, 2] = boxes_xyxy[:, 2] - boxes_xyxy[:, 0]
        boxes_xywh[:, 3] = boxes_xyxy[:, 3] - boxes_xyxy[:, 1]

        return boxes_xywh

    # def load_class_names(self, namesfile):
    #     with open(namesfile, 'r', encoding='utf8') as fp:
    #         class_names = [line.strip() for line in fp.readlines()]
    #     return class_names

#
# def demo():
#     import os
#     from vizer.draw import draw_boxes
#
#     yolo = YOLOv3("cfg/yolo_v3.cfg", "weight/yolov3.weights", "cfg/coco.names")
#     print("yolo.size =", yolo.size)
#     root = "./demo"
#     resdir = os.path.join(root, "results")
#     os.makedirs(resdir, exist_ok=True)
#     files = [os.path.join(root, file) for file in os.listdir(root) if file.endswith('.jpg')]
#     files.sort()
#     for filename in files:
#         img = cv2.imread(filename)
#         img = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)
#         bbox, cls_conf, cls_ids = yolo(img)
#
#         if bbox is not None:
#             img = draw_boxes(img, bbox, cls_ids, cls_conf, class_name_map=yolo.class_names)
#         # save results
#         cv2.imwrite(os.path.join(resdir, os.path.basename(filename)), img[:, :, (2, 1, 0)])
#         # imshow
#         # cv2.namedWindow("yolo", cv2.WINDOW_NORMAL)
#         # cv2.resizeWindow("yolo", 600,600)
#         # cv2.imshow("yolo",res[:,:,(2,1,0)])
#         # cv2.waitKey(0)
#
#
# if __name__ == "__main__":
#     demo()
