import io
from PIL import Image

print("Python Scripts File - Direct Command")


# def func_wo_params():
#     print("Python Scripts File - Function w.o. Params")


def func_w_params(param):
    res = "Python Scripts File - Function w. Params: %s" % param
    print(res)
    return res


def buffer_2_image(img_buffer: bytes, is_debug: bool = False):
    """
    for the binary buffer of a given image, convert to a <PIL.Image> object
    :param img_buffer:      image binary buffer, typed <bytes>
    :param is_debug:        flag indicating whether is debug mode. `True` if so, `False` otherwise
    :return:                corresponding <PIL.Image> object
    """
    # Reference: https://stackoverflow.com/a/32908899

    if is_debug is True:
        print("buffer:", type(img_buffer), len(img_buffer))
    img = Image.open(io.BytesIO(img_buffer))

    if is_debug is True:
        img.show()
    return img


def buffer_2_image_file(img_buffer: bytes):
    img = buffer_2_image(img_buffer=img_buffer)
    img.save("img1.png", "PNG")
    return "Success"


class ClassObject:
    def __init__(self, msg, cnt):
        self.msg = msg
        self.cnt = cnt

    def class_met_w_params(self, param):
        res = "%s, %d, %r" % (self.msg, self.cnt, param)
        return res
