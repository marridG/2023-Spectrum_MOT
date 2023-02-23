//
// Created by GuoZL on 2023-02-23.
//

#define PY_SSIZE_T_CLEAN

#include <Python.h>
#include "py_examples.h"

#include <bits/stdc++.h>

#include "utils.h"

using namespace std;


#define DEBUG

/**
 * directly run Python commands. e.g.:
 * <ul>
 *  <li>`import sys`</li>
 *  <li>`print("hello world")`</li>
 * </ul>
 */
void RunPyCommand() {

    // exec if run alone
    // // ===== initialize Python interpreter =====
    // cout << "Initializing ... " << flush;
    // Py_Initialize();
    // if (!Py_IsInitialized()) {
    //     cout << "[ERROR] Python is NOT Properly Initialized. Exit with Error." << endl;
    //     exit(1);
    // }
    // cout << "Py-Init ... " << flush;
    // // initialize Python system directory
    // PyRun_SimpleString("import sys\n"
    //                    "import os");
    // PyRun_SimpleString("sys.path.append(os.path.join(os.path.dirname(os.getcwd()), 'MOT'))");
    // cout << "Sys Path ... Done" << flush;
    // // PyRun_SimpleString("print(sys.path)");


    // run a simple Python command
    cout << "\n" << "Run a Simple Command:" << endl;
    PyRun_SimpleString("print('Python/C: Hello World')\n");


    // shut down the interpreter
    // Py_Finalize();  // exec when func is run alone
    cout << "==> EXIT" << endl;

}


/**
 * import a Python file as module & run its "global" commands
 */
void RunPyFile() {

    // exec if run alone
    // // ===== initialize Python interpreter =====
    // cout << "Initializing ... " << flush;
    // Py_Initialize();
    // if (!Py_IsInitialized()) {
    //     cout << "[ERROR] Python is NOT Properly Initialized. Exit with Error." << endl;
    //     exit(1);
    // }
    // cout << "Py-Init ... " << flush;
    // // initialize Python system directory
    // PyRun_SimpleString("import sys\n"
    //                    "import os");
    // PyRun_SimpleString("sys.path.append(os.path.join(os.path.dirname(os.getcwd()), 'MOT'))");
    // cout << "Sys Path ... Done" << flush;
    // // PyRun_SimpleString("print(sys.path)");


    // run a Python file
    cout << "\n" << "Call a .py File:" << endl;
    PyImport_ImportModule("hello_world"); // hello_world.py


    // shut down the interpreter
    // Py_Finalize();  // exec when func is run alone
    cout << "==> EXIT" << endl;

}


/**
 * run a function (with params) of a Python file
 */
void RunPyFileFunc() {

    // exec if run alone
    // // ===== initialize Python interpreter =====
    // cout << "Initializing ... " << flush;
    // Py_Initialize();
    // if (!Py_IsInitialized()) {
    //     cout << "[ERROR] Python is NOT Properly Initialized. Exit with Error." << endl;
    //     exit(1);
    // }
    // cout << "Py-Init ... " << flush;
    // // initialize Python system directory
    // PyRun_SimpleString("import sys\n"
    //                    "import os");
    // PyRun_SimpleString("sys.path.append(os.path.join(os.path.dirname(os.getcwd()), 'MOT'))");
    // cout << "Sys Path ... Done" << flush;
    // // PyRun_SimpleString("print(sys.path)");


    // declare pointer variables
    PyObject *p_name, *p_module, *p_class, *p_class_obj, *p_func;
    PyObject *p_args, *p_return;


    cout << "\n" << "Call a Function of a Module:" << endl;
    // find & import module
    p_module = PyImport_ImportModule("hello_world");
    if (!p_module) {  // error handling
        PyErr_Print();
        fprintf(stderr, "[ERROR] Failed to Load Module \"%s\"\n", "hello_world");
        exit(1);
    }
    // find function in module
    p_func = PyObject_GetAttrString(p_module, "func_w_params");
    if (!p_func || !PyCallable_Check(p_func)) {  // error handling
        Py_DECREF(p_module);
        PyErr_Print();
        fprintf(stderr, "[ERROR] Failed to Load Function \"%s\" (of Module \"%s\")\n", "do", "func_w_params");
        exit(1);
    }

    p_args = PyTuple_New(1); // arg `size` is the number of params to-be-passed to the Python function
    int val = 1;
    PyTuple_SetItem(p_args, 0, Py_BuildValue("i", val));  // set idx=0; format code `"i"` is <int> in Python & C++
    // call the function
    p_return = PyObject_CallObject(p_func, p_args);
    Py_DECREF(p_args);
    if (!p_return) {  // error handling
        Py_DECREF(p_func);
        Py_DECREF(p_module);
        PyErr_Print();
        fprintf(stderr, "Call (with Args) Failed\n");
        exit(1);
    }
    printf("Result of call: \"%s\"\n", PyUnicode_AsUTF8(p_return));
    Py_DECREF(p_return);
    Py_DECREF(p_func);
    Py_DECREF(p_module);


    // shut down the interpreter
    // Py_Finalize();  // exec when func is run alone
    cout << "==> EXIT" << endl;
}


/**
 * run a member function of an instantiated (with params) class object of a Python file
 */
void RunPyFileClsFunc() {
    // exec if run alone
    // // ===== initialize Python interpreter =====
    // cout << "Initializing ... " << flush;
    // Py_Initialize();
    // if (!Py_IsInitialized()) {
    //     cout << "[ERROR] Python is NOT Properly Initialized. Exit with Error." << endl;
    //     exit(1);
    // }
    // cout << "Py-Init ... " << flush;
    // // initialize Python system directory
    // PyRun_SimpleString("import sys\n"
    //                    "import os");
    // PyRun_SimpleString("sys.path.append(os.path.join(os.path.dirname(os.getcwd()), 'MOT'))");
    // cout << "Sys Path ... Done" << flush;
    // // PyRun_SimpleString("print(sys.path)");

    // declare pointer variables
    PyObject *p_name, *p_module, *p_class, *p_class_obj, *p_func;
    PyObject *p_args, *p_return;


    cout << "\n" << "Call a Method of a Class:" << endl;
    // find & import module
    p_module = PyImport_ImportModule("hello_world");
    if (!p_module) {  // error handling
        PyErr_Print();
        fprintf(stderr, "[ERROR] Failed to Load Module \"%s\"\n", "hello_world");
        exit(1);
    }
    // find class in module
    p_class = PyObject_GetAttrString(p_module, "ClassObject");
    if (!p_class || !PyCallable_Check(p_class)) {  // error handling
        Py_DECREF(p_module);
        PyErr_Print();
        fprintf(stderr, "[ERROR] Failed to Load Class \"%s\"\n", "ClassObject");
        exit(1);
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
        exit(1);
    }
    // find method of class obj
    p_func = PyObject_GetAttrString(p_class_obj, "class_met_w_params");
    if (!p_func || !PyCallable_Check(p_func)) {  // error handling
        Py_DECREF(p_class_obj);
        Py_DECREF(p_class);
        Py_DECREF(p_module);
        PyErr_Print();
        fprintf(stderr, "[ERROR] Failed to Find Method \"%s\"\n", "class_met_w_params");
        exit(1);
    }
    // calling method of class obj
    p_args = PyTuple_New(1); // arg `size` is the number of params to-be-passed to the Python function
    PyTuple_SetItem(p_args, 0, Py_BuildValue("i", 1));  // set idx=0; format code `"i"` is <int> in Python & C++
    p_return = PyObject_CallObject(p_func, p_args);
    Py_DECREF(p_args);
    if (!p_return) {
        Py_DECREF(p_class_obj);
        Py_DECREF(p_class);
        Py_DECREF(p_module);
        PyErr_Print();
        fprintf(stderr, "[ERROR] Failed to Call Method\n");
        exit(1);
    }
    printf("Result of call: \"%s\"\n", PyUnicode_AsUTF8(p_return));
    Py_DECREF(p_return);
    Py_DECREF(p_func);
    Py_DECREF(p_class_obj);
    Py_DECREF(p_class);
    Py_DECREF(p_module);


    // shut down the interpreter
    // Py_Finalize();  // exec when func is run alone
    cout << "==> EXIT" << endl;

}


/**
 * run a Python function, which takes image bytes from C++ and perform operations
 */
void RunPyPassImage() {

    // init image buffer (analog by reading from file)
    //      compatability alert: C++ 11 or up
    char *img_buffer;
    int img_size_byte;
    tie(img_buffer, img_size_byte) = MakeImageStreamBufferFromFile();
    cout << img_buffer << "=>" << img_size_byte << endl;


    // exec if run alone
    // // ===== initialize Python interpreter =====
    // cout << "Initializing ... " << flush;
    // Py_Initialize();
    // if (!Py_IsInitialized()) {
    //     cout << "[ERROR] Python is NOT Properly Initialized. Exit with Error." << endl;
    //     exit(1);
    // }
    // cout << "Py-Init ... " << flush;
    // // initialize Python system directory
    // PyRun_SimpleString("import sys\n"
    //                    "import os");
    // PyRun_SimpleString("sys.path.append(os.path.join(os.path.dirname(os.getcwd()), 'MOT'))");
    // cout << "Sys Path ... Done" << flush;
    // // PyRun_SimpleString("print(sys.path)");

    // declare pointer variables
    PyObject *p_name, *p_module, *p_class, *p_class_obj, *p_func;
    PyObject *p_args, *p_return;

    cout << "\n" << "Call a Function of a Module:" << endl;
    // find & import module
    p_module = PyImport_ImportModule("hello_world");
    if (!p_module) {  // error handling
        PyErr_Print();
        fprintf(stderr, "[ERROR] Failed to Load Module \"%s\"\n", "hello_world");
        exit(1);
    }
    // find function in module
    p_func = PyObject_GetAttrString(p_module, "buffer_2_image_file");
    if (!p_func || !PyCallable_Check(p_func)) {  // error handling
        Py_DECREF(p_module);
        PyErr_Print();
        fprintf(stderr, "[ERROR] Failed to Load Function \"%s\" (of Module \"%s\")\n", "do",
                " buffer_2_image_file");
        exit(1);
    }

    p_args = Py_BuildValue("(y#)", img_buffer, img_size_byte);
    // call the function
    p_return = PyObject_CallObject(p_func, p_args);
    Py_DECREF(p_args);
    if (!p_return) {  // error handling
        Py_DECREF(p_func);
        Py_DECREF(p_module);
        PyErr_Print();
        fprintf(stderr, "Call (with Args) Failed\n");
        exit(1);
    }
    printf("Result of call: \"%s\"\n", PyUnicode_AsUTF8(p_return));
    Py_DECREF(p_return);
    Py_DECREF(p_func);
    Py_DECREF(p_module);

    // shut down the interpreter
    // Py_Finalize();  // exec when func is run alone
    cout << "==> EXIT" << endl;

}


/**
 * run a Python function, which returns an int-array shaped (N,4), indicating bbox
 */
void RunPyGetArray() {
    static const int LEN_BBOX_FEATURE = 4;

    // exec if run alone
    // // ===== initialize Python interpreter =====
    // cout << "Initializing ... " << flush;
    // Py_Initialize();
    // if (!Py_IsInitialized()) {
    //     cout << "[ERROR] Python is NOT Properly Initialized. Exit with Error." << endl;
    //     exit(1);
    // }
    // cout << "Py-Init ... " << flush;
    // // initialize Python system directory
    // PyRun_SimpleString("import sys\n"
    //                    "import os");
    // PyRun_SimpleString("sys.path.append(os.path.join(os.path.dirname(os.getcwd()), 'MOT'))");
    // cout << "Sys Path ... Done" << flush;
    // // PyRun_SimpleString("print(sys.path)");

    // declare pointer variables
    PyObject *p_name, *p_module, *p_class, *p_class_obj, *p_func;
    PyObject *p_args, *p_return;
    PyObject *p_temp;

    cout << "\n" << "Call a Function of a Module:" << endl;
    // find & import module
    p_module = PyImport_ImportModule("hello_world");
    if (!p_module) {  // error handling
        PyErr_Print();
        fprintf(stderr, "[ERROR] Failed to Load Module \"%s\"\n", "hello_world");
        exit(1);
    }
    // find function in module
    p_func = PyObject_GetAttrString(p_module, "get_bbox");
    if (!p_func || !PyCallable_Check(p_func)) {  // error handling
        Py_DECREF(p_module);
        PyErr_Print();
        fprintf(stderr, "[ERROR] Failed to Load Function \"%s\"\n", "get_bbox");
        exit(1);
    }

    cout << "ready to call" << endl;
    // call the function
    p_return = PyObject_CallObject(p_func, nullptr);
    if (!p_return) {  // error handling
        Py_DECREF(p_func);
        Py_DECREF(p_module);
        PyErr_Print();
        fprintf(stderr, "Call (without Args) Failed\n");
        exit(1);
    }
    // handles the returned (N,LEN_BBOX_FEATURE) list. Reference: https://stackoverflow.com/a/936702
    int obj_cnt = (int) PyList_Size(p_return);
    if (0 > obj_cnt) {
        Py_DECREF(p_return);
        Py_DECREF(p_func);
        Py_DECREF(p_module);
        PyErr_Print();
        fprintf(stderr, "[ERROR] Failed to Parse Call Result. Obj_Cnt < 0.\n");
        exit(1);
    }
    auto obj_arr = new int[obj_cnt][LEN_BBOX_FEATURE];  // compatability alert: C++ 11 or up
    for (int _row_idx = 0; _row_idx <= obj_cnt - 1; _row_idx++) {
        p_temp = PyList_GetItem(p_return, _row_idx);
        for (int _col_idx = 0; _col_idx <= LEN_BBOX_FEATURE - 1; _col_idx++) {
            obj_arr[_row_idx][_col_idx] = (int) PyLong_AsLong(PyList_GetItem(p_temp, _col_idx));
        }
        Py_DECREF(p_temp);
    }
    printf("Result of call - (%d, %d) array:\n", obj_cnt, LEN_BBOX_FEATURE);
    for (int _row_idx = 0; _row_idx <= obj_cnt - 1; _row_idx++) {
        cout << "\tobj #" << _row_idx << ": (" << flush;
        for (int _col_idx = 0; _col_idx <= LEN_BBOX_FEATURE - 1; _col_idx++) {
            cout << obj_arr[_row_idx][_col_idx] << "," << flush;
        }
        cout << ")" << endl;
    }
    Py_DECREF(p_return);
    delete[] obj_arr;
    Py_DECREF(p_func);
    Py_DECREF(p_module);

    // shut down the interpreter
    // Py_Finalize();  // exec when func is run alone
    cout << "==> EXIT" << endl;

}
