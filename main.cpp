#define PY_SSIZE_T_CLEAN

#include <Python.h>
// #include <bits/stdc++.h>
#include <iostream>

using namespace std;

#define DEBUG

int main() {

    // ===== initialize Python interpreter =====
    cout << "Initializing ... " << flush;
    Py_Initialize();
    if (!Py_IsInitialized()) {
        cout << "[ERROR] Python is NOT Properly Initialized. Exit with Error." << endl;
        return 1;
    }
    cout << "Py-Init ... " << flush;
    // initialize Python system directory
    PyRun_SimpleString("import sys\n"
                       "import os");
    PyRun_SimpleString("sys.path.append(os.path.join(os.path.dirname(os.getcwd()), 'MOT'))");
    cout << "Sys Path ... Done" << flush;
    // PyRun_SimpleString("print(sys.path)");

    // declare pointer variables
    PyObject *pName, *pModule, *pCls, *pClsInst, *pFunc;
    PyObject *pArgs, *pRet;

    // [1] run a simple Python command
    {
        cout << "\n" << "Run a Simple Command:" << endl;
        PyRun_SimpleString("print('Python/C: Hello World')\n");
    }

    // [2] call .py file
    {
        cout << "\n" << "Call a .py File:" << endl;
        PyImport_ImportModule("hello_world"); // hello_world.py
    }

    // [3] call a function of a module
    {
        cout << "\n" << "Call a Function of a Module:" << endl;
        // find & import module
        pModule = PyImport_ImportModule("hello_world");
        if (!pModule) {  // error handling
            PyErr_Print();
            fprintf(stderr, "[ERROR] Failed to Load Module \"%s\"\n", "hello_world");
            return 1;
        }
        // find function in module
        pFunc = PyObject_GetAttrString(pModule, "func_w_params");
        if (!pFunc || !PyCallable_Check(pFunc)) {  // error handling
            Py_DECREF(pModule);
            PyErr_Print();
            fprintf(stderr, "[ERROR] Failed to Load Function \"%s\" (of Module \"%s\")\n", "do", "func_w_params");
            return 1;
        }

        pArgs = PyTuple_New(1); // arg `size` is the number of params to-be-passed to the Python function
        PyTuple_SetItem(pArgs, 0, Py_BuildValue("i", 1));  // set idx=0; format code `"i"` is <int> in Python & C++
        // call the function
        pRet = PyObject_CallObject(pFunc, pArgs);
        Py_DECREF(pArgs);
        if (!pRet) {  // error handling
            Py_DECREF(pFunc);
            Py_DECREF(pModule);
            PyErr_Print();
            fprintf(stderr, "Call (with Args) Failed\n");
            return 1;
        }
        printf("Result of call: \"%s\"\n", PyUnicode_AsUTF8(pRet));
        Py_DECREF(pRet);
        Py_DECREF(pFunc);
        Py_DECREF(pModule);
    }

    // [4] call a method of a class
    cout << "\n" << "Call a Method of a Class:" << endl;
    // find & import module
    pModule = PyImport_ImportModule("hello_world");
    if (!pModule) {  // error handling
        PyErr_Print();
        fprintf(stderr, "[ERROR] Failed to Load Module \"%s\"\n", "hello_world");
        return 1;
    }
    // find class in module
    pCls = PyObject_GetAttrString(pModule, "ClassObject");
    if (!pCls || !PyCallable_Check(pCls)) {  // error handling
        Py_DECREF(pModule);
        PyErr_Print();
        fprintf(stderr, "[ERROR] Failed to Load Class \"%s\"\n", "ClassObject");
        return 1;
    }
    // instantiate class obj, passing args
    pArgs = Py_BuildValue("si", "hello", 42);
    pClsInst = PyObject_CallObject(pCls, pArgs);
    Py_DECREF(pArgs);
    if (!pClsInst) {  // error handling
        Py_DECREF(pCls);
        Py_DECREF(pModule);
        PyErr_Print();
        fprintf(stderr, "[ERROR] Failed to Instantiate an Object");
        return 1;
    }
    // find method of class obj
    pFunc = PyObject_GetAttrString(pClsInst, "class_met_w_params");
    if (!pFunc || !PyCallable_Check(pFunc)) {  // error handling
        Py_DECREF(pClsInst);
        Py_DECREF(pCls);
        Py_DECREF(pModule);
        PyErr_Print();
        fprintf(stderr, "[ERROR] Failed to Find Method \"%s\"\n", "class_met_w_params");
        return 1;
    }
    // calling method of class obj
    pArgs = PyTuple_New(1); // arg `size` is the number of params to-be-passed to the Python function
    PyTuple_SetItem(pArgs, 0, Py_BuildValue("i", 1));  // set idx=0; format code `"i"` is <int> in Python & C++
    pRet = PyObject_CallObject(pFunc, pArgs);
    Py_DECREF(pArgs);
    if (!pRet) {
        Py_DECREF(pClsInst);
        Py_DECREF(pCls);
        Py_DECREF(pModule);
        PyErr_Print();
        fprintf(stderr, "[ERROR] Failed to Call Method\n");
        return 1;
    }
    printf("Result of call: \"%s\"\n", PyUnicode_AsUTF8(pRet));
    Py_DECREF(pRet);
    Py_DECREF(pFunc);
    Py_DECREF(pClsInst);
    Py_DECREF(pCls);
    Py_DECREF(pModule);



    // shut down the interpreter
    Py_Finalize();

    return 0;
}
