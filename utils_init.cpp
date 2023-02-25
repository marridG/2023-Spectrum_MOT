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
    cout << "Initializing Python ... " << flush;
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


/**
 * init the tracker. <br/>
 * ImplSpec:
 * <ol>
 *  <li>find the Python tracker class from file;</li>
 *  <li>instantiate the Python tracker class;</li>
 *  <li>for the tracker instance, find the detection function;</li>
 *  <li>for the tracker instance, find the tracking function;</li>
 * </ol>
 * API Note:
 * <ul>
 *  <li>[Note] "de-allocation" (i.e. ref cnt dec) of all the Python-object reference pointers <b>as params</b>:
 *      <ul>
 *          <li>function exec successfully: SAFE TO IGNORE DE-ALLOCATION. no need to de-allocated.</li>
 *          <li>function exec failed: SAFE TO IGNORE DE-ALLOCATION. de-allocation of possibly instantiated pointers has been performed.</li>
 *      </ul>
 *  </li>
 * </ul>
 * @param p_module          (param as reference) destination to store the located Python entry file
 * @param p_class           (param as reference) destination to store the located Python tracker class
 * @param p_class_obj       (param as reference) destination to store the instantiated Python tracker class instance
 * @param p_func_detect     (param as reference) destination to store the located detection function
 * @param p_func_track      (param as reference) destination to store the located tracking function
 * @return                  `0` if successful, `-1` or raised errors otherwise
 */
int InitOnlineTracker(PyObject *&p_module, PyObject *&p_class, PyObject *&p_class_obj,
                      PyObject *&p_func_detect, PyObject *&p_func_track) {

    // find & import module
    p_module = PyImport_ImportModule("tracker_online");
    if (!p_module) {  // error handling
        PyErr_Print();
        fprintf(stderr, "[ERROR] Failed to Load Module \"%s\"\n", "tracker_online");
        return -1;
    }

    // find class in module
    p_class = PyObject_GetAttrString(p_module, "TrackerOnline");
    if (!p_class || !PyCallable_Check(p_class)) {  // error handling
        Py_DECREF(p_module);
        PyErr_Print();
        fprintf(stderr, "[ERROR] Failed to Load Class \"%s\"\n", "TrackerOnline");
        return -1;
    }

    // instantiate class obj, passing args
    /*PyObject *p_args;
    string val1 = "hello";
    int val2 = 42;
    p_args = Py_BuildValue("si", val1.c_str(), val2);
    p_class_obj = PyObject_CallObject(p_class, p_args);
    Py_DECREF(p_args);*/
    p_class_obj = PyObject_CallObject(p_class, nullptr);
    if (!p_class_obj) {  // error handling
        Py_DECREF(p_class);
        Py_DECREF(p_module);
        PyErr_Print();
        fprintf(stderr, "[ERROR] Failed to Instantiate an Object");
        return -1;
    }
    /*if ((0 >= (int) Py_REFCNT(p_module)) || (0 >= (int) Py_REFCNT(p_class)) || (0 >= (int) Py_REFCNT(p_class_obj))) {
        PyErr_Print();
        fprintf(stderr, "[ERROR] Failed to Instantiate the Online Tracker Object."
                        " Ref-Cnt>0 Assertion Failed for module/class/instance\n");
        return -1;
    }*/

    cout << flush << "Online Tracker Instantiated ... " << flush;

    // find func: detection
    p_func_detect = PyObject_GetAttrString(p_class_obj, "detect");
    if (!p_func_detect || !PyCallable_Check(p_func_detect)) {  // error handling
        Py_DECREF(p_class_obj);
        Py_DECREF(p_class);
        Py_DECREF(p_module);
        PyErr_Print();
        fprintf(stderr, "[ERROR] Failed to Find Method \"%s\"\n", "detect");
        return -1;
    }
    cout << "Detection Function Located ... " << flush;
    // find func: tracking
    p_func_track = PyObject_GetAttrString(p_class_obj, "track");
    if (!p_func_track || !PyCallable_Check(p_func_track)) {  // error handling
        Py_DECREF(p_func_detect);
        Py_DECREF(p_class_obj);
        Py_DECREF(p_class);
        Py_DECREF(p_module);
        PyErr_Print();
        fprintf(stderr, "[ERROR] Failed to Find Method \"%s\"\n", "track");
        return -1;
    }
    cout << "Tracking Function Located ... ALL Init Done." << flush;

    return 0;
}