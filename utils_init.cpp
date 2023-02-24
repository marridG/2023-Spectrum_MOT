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