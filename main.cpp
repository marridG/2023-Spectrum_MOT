#define PY_SSIZE_T_CLEAN

#include <Python.h>
#include <bits/stdc++.h>

#include "utils.h"

using namespace std;

#define DEBUG

// Naming Convention: [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html#Naming)

int main() {

    char *img_buffer;
    int img_size_byte;
    tie(img_buffer, img_size_byte) = MakeImageStreamBufferFromFile();
    cout << img_buffer << "=>" << img_size_byte << endl;



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
    PyObject *p_name, *p_module, *p_class, *p_class_obj, *p_func;
    PyObject *p_args, *p_return;


    {
        cout << "\n" << "Call a Function of a Module:" << endl;
        // find & import module
        p_module = PyImport_ImportModule("hello_world");
        if (!p_module) {  // error handling
            PyErr_Print();
            fprintf(stderr, "[ERROR] Failed to Load Module \"%s\"\n", "hello_world");
            return 1;
        }
        // find function in module
        p_func = PyObject_GetAttrString(p_module, "buffer_2_image_file");
        if (!p_func || !PyCallable_Check(p_func)) {  // error handling
            Py_DECREF(p_module);
            PyErr_Print();
            fprintf(stderr, "[ERROR] Failed to Load Function \"%s\" (of Module \"%s\")\n", "do",
                    " buffer_2_image_file");
            return 1;
        }

        p_args = Py_BuildValue("y#,i", img_buffer, img_size_byte);
        // call the function
        p_return = PyObject_CallObject(p_func, p_args);
        Py_DECREF(p_args);
        if (!p_return) {  // error handling
            Py_DECREF(p_func);
            Py_DECREF(p_module);
            PyErr_Print();
            fprintf(stderr, "Call (with Args) Failed\n");
            return 1;
        }
        printf("Result of call: \"%s\"\n", PyUnicode_AsUTF8(p_return));
        Py_DECREF(p_return);
        Py_DECREF(p_func);
        Py_DECREF(p_module);
    }



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
        p_module = PyImport_ImportModule("hello_world");
        if (!p_module) {  // error handling
            PyErr_Print();
            fprintf(stderr, "[ERROR] Failed to Load Module \"%s\"\n", "hello_world");
            return 1;
        }
        // find function in module
        p_func = PyObject_GetAttrString(p_module, "func_w_params");
        if (!p_func || !PyCallable_Check(p_func)) {  // error handling
            Py_DECREF(p_module);
            PyErr_Print();
            fprintf(stderr, "[ERROR] Failed to Load Function \"%s\" (of Module \"%s\")\n", "do", "func_w_params");
            return 1;
        }

        p_args = PyTuple_New(1); // arg `size` is the number of params to-be-passed to the Python function
        PyTuple_SetItem(p_args, 0, Py_BuildValue("i", 1));  // set idx=0; format code `"i"` is <int> in Python & C++
        // call the function
        p_return = PyObject_CallObject(p_func, p_args);
        Py_DECREF(p_args);
        if (!p_return) {  // error handling
            Py_DECREF(p_func);
            Py_DECREF(p_module);
            PyErr_Print();
            fprintf(stderr, "Call (with Args) Failed\n");
            return 1;
        }
        printf("Result of call: \"%s\"\n", PyUnicode_AsUTF8(p_return));
        Py_DECREF(p_return);
        Py_DECREF(p_func);
        Py_DECREF(p_module);
    }

    // [4] call a method of a class
    {
        cout << "\n" << "Call a Method of a Class:" << endl;
        // find & import module
        p_module = PyImport_ImportModule("hello_world");
        if (!p_module) {  // error handling
            PyErr_Print();
            fprintf(stderr, "[ERROR] Failed to Load Module \"%s\"\n", "hello_world");
            return 1;
        }
        // find class in module
        p_class = PyObject_GetAttrString(p_module, "ClassObject");
        if (!p_class || !PyCallable_Check(p_class)) {  // error handling
            Py_DECREF(p_module);
            PyErr_Print();
            fprintf(stderr, "[ERROR] Failed to Load Class \"%s\"\n", "ClassObject");
            return 1;
        }
        // instantiate class obj, passing args
        p_args = Py_BuildValue("si", "hello", 42);
        p_class_obj = PyObject_CallObject(p_class, p_args);
        Py_DECREF(p_args);
        if (!p_class_obj) {  // error handling
            Py_DECREF(p_class);
            Py_DECREF(p_module);
            PyErr_Print();
            fprintf(stderr, "[ERROR] Failed to Instantiate an Object");
            return 1;
        }
        // find method of class obj
        p_func = PyObject_GetAttrString(p_class_obj, "class_met_w_params");
        if (!p_func || !PyCallable_Check(p_func)) {  // error handling
            Py_DECREF(p_class_obj);
            Py_DECREF(p_class);
            Py_DECREF(p_module);
            PyErr_Print();
            fprintf(stderr, "[ERROR] Failed to Find Method \"%s\"\n", "class_met_w_params");
            return 1;
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
            return 1;
        }
        printf("Result of call: \"%s\"\n", PyUnicode_AsUTF8(p_return));
        Py_DECREF(p_return);
        Py_DECREF(p_func);
        Py_DECREF(p_class_obj);
        Py_DECREF(p_class);
        Py_DECREF(p_module);
    }



    // shut down the interpreter
    Py_Finalize();

    return 0;
}
