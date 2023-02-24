#define PY_SSIZE_T_CLEAN

#include <Python.h>
#include <bits/stdc++.h>

#include "utils.h"
#include "py_examples.h"

using namespace std;

#define DEBUG

// 1. Naming Convention: [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html#Naming)
int main() {

    InitPython();

    RunPyPassImage();
    RunPyGetArray();

    RunPyCommand();
    RunPyFile();
    RunPyFileFunc();
    RunPyFileClsFunc();

    ShutDownPython();

    return 0;
}
