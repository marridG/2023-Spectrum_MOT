print("Python Scripts File - Direct Command")


# def func_wo_params():
#     print("Python Scripts File - Function w.o. Params")


def func_w_params(param):
    res = "Python Scripts File - Function w. Params: %s" % param
    print(res)
    return res


class ClassObject:
    def __init__(self, msg, cnt):
        self.msg = msg
        self.cnt = cnt

    def class_met_w_params(self, param):
        res = "%s, %d, %r" % (self.msg, self.cnt, param)
        return res
