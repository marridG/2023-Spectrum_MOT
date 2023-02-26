#define PY_SSIZE_T_CLEAN

#include <Python.h>
#include <bits/stdc++.h>

#include "utils.h"
#include "py_examples.h"

using namespace std;

// #define DEBUG

// 1. Naming Convention: [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html#Naming)
// 2. compatibility alert: search for "compatability alert: "
// 3. possible improvements: search for "future: "
// 4. other flags: search for: "path alert: "
int main() {

    InitPython();

    // // examples: (1) image C->Py; (2) array Py->C
    // RunPyPassImage();
    // RunPyGetArray();
    //
    // // examples
    // RunPyCommand();
    // RunPyFile();
    // RunPyFileFunc();
    // RunPyFileClsFunc();

    // declare pointer variables
    PyObject *p_name, *p_module, *p_class, *p_class_obj, *p_func_detect, *p_func_track;
    PyObject *p_args, *p_return;
    PyObject *p_temp;

    if (0 != InitOnlineTracker(p_module, p_class, p_class_obj, p_func_detect, p_func_track)) {
        fprintf(stderr, "[ERROR] Failed to Init Online Tracker.\n");
        return -1;
    }
#ifdef DEBUG
    // check ref count. expecting >0, >0, 1
    cout << "(main) ref cnt: "
         << Py_REFCNT(p_module) << "," << Py_REFCNT(p_class) << "," << Py_REFCNT(p_class_obj) << endl;
    // // if the following codes are run, expected outputs are (0,0,0), but get (1,4,0) even iterated
    // Py_DECREF(p_class_obj);
    // Py_DECREF(p_class);
    // Py_DECREF(p_module);
    // cout << "(main) ref cnt (desrtucted): "
    //      << Py_REFCNT(p_module) << "," << Py_REFCNT(p_class) << "," << Py_REFCNT(p_class_obj) << endl;
#endif

    for (int i = 0; i <= 5; i++) {

        // init image buffer (analog by reading from file)
        //      compatability alert: C++ 11 or up
        char *img_buffer;  // remember to destruct!!
        int img_size_byte;
        tie(img_buffer, img_size_byte) = MakeImageStreamBufferFromFile("../../test.jpg");
        cout << img_buffer << "=>" << img_size_byte << endl;

        // exec detection: image ==> bbox
        //      compatability alert: C++ 11 or up
        auto _detect_res = ExecDetect(p_func_detect, img_buffer, img_size_byte);
        auto obj_arr = get<0>(_detect_res);  // int(*)[obj_dim],
        int obj_cnt = get<1>(_detect_res);
        int obj_dim = get<2>(_detect_res);
#ifdef DEBUG
        printf("Detection Results - (%d, %d) array of x_top_left/y_top_left/w/h:\n", obj_cnt, obj_dim);
        for (int _row_idx = 0; _row_idx <= obj_cnt - 1; _row_idx++) {
            cout << "\tobj #" << _row_idx << ": (" << flush;
            for (int _col_idx = 0; _col_idx <= obj_dim - 1; _col_idx++) {
                cout << obj_arr[_row_idx][_col_idx] << "," << flush;
            }
            cout << ")" << endl;
        }
#endif

        // exec track
        //      compatability alert: C++ 11 or up
        auto _track_res = ExecTrack(p_func_track);
        auto track_obj_arr = get<0>(_track_res);  // int(*)[obj_dim],
        int track_obj_cnt = get<1>(_track_res);
        int track_obj_dim = get<2>(_track_res);

        cout << endl << endl;
    }

    // Py_XDECREF(p_temp);
    // Py_XDECREF(p_return);
    // Py_XDECREF(p_args);
    Py_XDECREF(p_func_track);
    Py_XDECREF(p_func_detect);
    Py_XDECREF(p_class_obj);
    Py_XDECREF(p_class);
    Py_XDECREF(p_module);
    // Py_XDECREF(p_name);

    ShutDownPython();

    return 0;
}
