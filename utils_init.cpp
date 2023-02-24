//
// Created by GuoZL on 2023-02-24.
//

#define PY_SSIZE_T_CLEAN

#include <Python.h>
#include "utils.h"
#include <bits/stdc++.h>

using namespace std;


#define DEBUG


/**
 * init Python interpreter and add search path
 * @return 0 on success; -1 or raised errors otherwise
 */
int InitPython() {
    // 1. Init Python Interpreter ONCE for all. Otherwise:
    //      `import numpy` multiple times will raise `Can only map one python type to DType.` error'
    //      [related problems](https://stackoverflow.com/questions/59314230/python-c-api-crashes-on-import-numpy-when-initilizing-multiple-times)

    // ===== initialize Python interpreter =====
    cout << "Initializing ... " << flush;
    Py_Initialize();
    if (!Py_IsInitialized()) {
        fprintf(stderr, "[ERROR] Python Interpreter is NOT Properly Initialized.\n");
        return -1;
    }
    cout << "Py-Init ... " << flush;

    // initialize Python system directory
    int return_val = 0;
    return_val = PyRun_SimpleString("import sys\n"
                                    "import os");
    if (0 != return_val) {
        fprintf(stderr, "[ERROR] Python Path-Init-Modules Imports Failed.\n");
        return -1;
    }
    return_val = PyRun_SimpleString("sys.path.append(os.path.join(os.path.dirname(os.getcwd()), 'MOT'))");
    if (0 != return_val) {
        fprintf(stderr, "[ERROR] Python Add Path (`MOT`) Failed.\n");
        return -1;
    }
    return_val = PyRun_SimpleString("sys.path.append(os.path.join(os.path.dirname(os.getcwd()), 'MOT', 'tracker'))");
    if (0 != return_val) {
        fprintf(stderr, "[ERROR] Python Add Path (`MOT/tracker`) Failed.\n");
        return -1;
    }

    cout << "Sys Path ... ALL Init Done." << endl << endl;
    // PyRun_SimpleString("print(sys.path)");
    return 0;
}


int InitOnlineTracker(PyObject *&p_module, PyObject *&p_class, PyObject *&p_class_obj) {

    PyObject *p_args;

    // find & import module
    p_module = PyImport_ImportModule("hello_world");
    if (!p_module) {  // error handling
        PyErr_Print();
        fprintf(stderr, "[ERROR] Failed to Load Module \"%s\"\n", "hello_world");
        return -1;
    }
    // find class in module
    p_class = PyObject_GetAttrString(p_module, "ClassObject");
    if (!p_class || !PyCallable_Check(p_class)) {  // error handling
        Py_DECREF(p_module);
        PyErr_Print();
        fprintf(stderr, "[ERROR] Failed to Load Class \"%s\"\n", "ClassObject");
        return -1;
    }
    // instantiate class obj, passing args
    string val1 = "hello";
    int val2 = 42;
    p_args = Py_BuildValue("si", val1.c_str(), val2);
    p_class_obj = PyObject_CallObject(p_class, p_args);
    Py_DECREF(p_args);
    if (!p_class_obj) {  // error handling
        Py_DECREF(p_class);
        Py_DECREF(p_module);
        PyErr_Print();
        fprintf(stderr, "[ERROR] Failed to Instantiate an Object");
        return -1;
    }
    if ((0 >= (int) Py_REFCNT(p_module)) || (0 >= (int) Py_REFCNT(p_class)) || (0 >= (int) Py_REFCNT(p_class_obj))) {
        PyErr_Print();
        fprintf(stderr, "[ERROR] Failed to Instantiate the Online Tracker Object\n");
        return -1;
    }

    cout << "Online Tracker Instantiated." << endl;
    return 0;
}