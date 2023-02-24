//
// Created by GuoZL on 2023-02-22.
//
#define PY_SSIZE_T_CLEAN

#include <Python.h>
#include<tuple>  // compatability alert: C++ 11 or up

#ifndef INC_202302_SPECTRUM_MOT_UTILS_H
#define INC_202302_SPECTRUM_MOT_UTILS_H

// init
int InitPython();
int InitOnlineTracker(PyObject *&p_module, PyObject *&p_class, PyObject *&p_class_obj);
// destruct
int ShutDownPython();

// inputs
std::tuple<char *, int> MakeImageStreamBufferFromFile();  // compatability alert: C++ 11 or up
#endif //INC_202302_SPECTRUM_MOT_UTILS_H
