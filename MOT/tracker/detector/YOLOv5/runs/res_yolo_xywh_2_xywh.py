import os
import cv2

RES_EXP_IDX = -1  # -1 for "exp", X for "expX"
RES_EXP_STR = "exp" if 0 > RES_EXP_IDX else ("exp%d" % RES_EXP_IDX)
RES_FN = "test"
LABEL_PATH = "detect/%s/labels/%s.txt" % (RES_EXP_STR, RES_FN)
IMG_PATH = "detect/%s/%s.jpg" % (RES_EXP_STR, RES_FN)

assert os.path.exists(LABEL_PATH)
assert os.path.exists(IMG_PATH)

IMG_WIDTH, IMG_HEIGHT = cv2.imread(IMG_PATH).shape[:2][::-1]
print("Width=%d, Height=%d" % (IMG_WIDTH, IMG_HEIGHT))

lines = []
with open(LABEL_PATH, "r") as f:
    _raw_lines = f.readlines()
    for __line in _raw_lines:
        __line = __line.strip()
        __line_split = __line.split()
        lines.append((int(__line_split[0]), float(__line_split[1]), float(__line_split[2]), float(__line_split[3]), float(__line_split[4])))

print(lines)

res = []
print("Detected Objects:")
for _obj_idx, _obj in enumerate(lines):
    _obj_cls, _obj_x_normed, _obj_y_normed, _obj_w_normed, _obj_h_normed = _obj
    _obj_x = int(_obj_x_normed * IMG_WIDTH)
    _obj_y = int(_obj_y_normed * IMG_HEIGHT)
    _obj_w = int(_obj_w_normed * IMG_WIDTH)
    _obj_h = int(_obj_h_normed * IMG_HEIGHT)
    print("\tIDX=%d, cls=%d, x=%d, y=%d, w=%d, h=%d" % (_obj_idx, _obj_cls, _obj_x, _obj_y, _obj_w, _obj_h))
print("==> END")

