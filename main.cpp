#define PY_SSIZE_T_CLEAN

#include <Python.h>
#include <bits/stdc++.h>

#include "utils.h"
#include "py_examples.h"

using namespace std;

#define DEBUG

// 1. Naming Convention: [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html#Naming)
// 2. Init Python Interpreter ONCE for all. Otherwise:
//      `import numpy` multiple times will raise `Can only map one python type to DType.` error'
//      [related problems](https://stackoverflow.com/questions/59314230/python-c-api-crashes-on-import-numpy-when-initilizing-multiple-times)

int main() {

    // ===== initialize Python interpreter =====
    cout << "Initializing ... " << flush;
    Py_Initialize();
    if (!Py_IsInitialized()) {
        cout << "[ERROR] Python is NOT Properly Initialized. Exit with Error." << endl;
        exit(1);
    }
    cout << "Py-Init ... " << flush;
    // initialize Python system directory
    PyRun_SimpleString("import sys\n"
                       "import os");
    PyRun_SimpleString("sys.path.append(os.path.join(os.path.dirname(os.getcwd()), 'MOT'))");
    cout << "Sys Path ... Done" << endl;
    // PyRun_SimpleString("print(sys.path)");

    RunPyPassImage();
    RunPyGetArray();

    RunPyCommand();
    RunPyFile();
    RunPyFileFunc();
    RunPyFileClsFunc();

    Py_Finalize();  // exec when func is run alone

    return 0;
}
