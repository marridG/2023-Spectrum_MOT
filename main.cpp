#define PY_SSIZE_T_CLEAN

#include <Python.h>
#include <bits/stdc++.h>

#include "utils.h"
#include "py_examples.h"

using namespace std;

// #define DEBUG

// 1. Naming Convention: [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html#Naming)
int main() {

    InitPython();

    // // examples: (1) image C->Py; (2) array Py->C
    // RunPyPassImage();
    // RunPyGetArray();
    //
    // // examples
    // RunPyCommand();
    // RunPyFile();
    // RunPyFileFunc();
    // RunPyFileClsFunc();

    // declare pointer variables
    PyObject *p_name, *p_module, *p_class, *p_class_obj, *p_func;
    PyObject *p_args, *p_return;
    PyObject *p_temp;

    InitOnlineTracker(p_module, p_class, p_class_obj);

#ifdef DEBUG
    // check ref count. expecting >0, >0, 1
    cout << "(main) ref cnt: "
         << Py_REFCNT(p_module) << "," << Py_REFCNT(p_class) << "," << Py_REFCNT(p_class_obj) << endl;
    // // if the following codes are run, expected outputs are (0,0,0), but get (1,4,0) even iterated
    // Py_DECREF(p_class_obj);
    // Py_DECREF(p_class);
    // Py_DECREF(p_module);
    // cout << "(main) ref cnt (desrtucted): "
    //      << Py_REFCNT(p_module) << "," << Py_REFCNT(p_class) << "," << Py_REFCNT(p_class_obj) << endl;
#endif


    // Py_XDECREF(p_temp);
    // Py_XDECREF(p_return);
    // Py_XDECREF(p_args);
    // Py_XDECREF(p_func);
    Py_XDECREF(p_class_obj);
    Py_XDECREF(p_class);
    Py_XDECREF(p_module);
    // Py_XDECREF(p_name);

    ShutDownPython();

    return 0;
}
