//
// Created by GuoZL on 2023-02-24.
//

#define PY_SSIZE_T_CLEAN

#include <Python.h>
#include "utils.h"
#include <bits/stdc++.h>

using namespace std;


#define DEBUG

int ShutDownPython() {
    // Py_Finalize()

    int return_val = 0;
    return_val = Py_FinalizeEx();
    if (0 != return_val) {
        fprintf(stderr, "[ERROR] Python Shut-Down Failed.\n");
        return -1;
    }
    return 0;
}
