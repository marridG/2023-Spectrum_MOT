#define PY_SSIZE_T_CLEAN

#include <Python.h>
#include <bits/stdc++.h>

#include "utils.h"
#include "py_examples.h"

using namespace std;

#define DEBUG

// Naming Convention: [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html#Naming)

int main() {

    RunPyPassImage();

    RunPyCommand();
    RunPyFile();
    RunPyFileFunc();
    RunPyFileClsFunc();

    return 0;
}
