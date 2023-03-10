//
// Created by GuoZL on 2023-02-22.
//
#define PY_SSIZE_T_CLEAN

#include <Python.h>
#include<tuple>  // compatability alert: C++ 11 or up
#include <iostream>
#include <memory>
// #include <string>
// #include <stdexcept>

#define DEBUG_UTILS_H


#ifndef INC_202302_SPECTRUM_MOT_UTILS_H
#define INC_202302_SPECTRUM_MOT_UTILS_H

// init
int InitPython();
int InitOnlineTracker(PyObject *&p_module, PyObject *&p_class, PyObject *&p_class_obj,
                      PyObject *&p_func_detect, PyObject *&p_func_track_sim, PyObject *&p_func_track_spectrum);
// destruct
int ShutDownPython();

// MOT - detect: for template-alike deduced functions, impl in header files only
/**
 * for a given image (as allocated binary-buffer instance), detect all the objects (& destruct the buffer instance).<br />
 * API notes:
 * <ul>
 *  <li>[Note] destruction of the input buffer instance (as param) will ALWAYS (success&failure) be committed</li>
 *  <li> [Note] if successful, access elements in the return 2d-array `obj_arr`:
 *      call `int elem = obj_arr[bbox_idx][feature_idx];`.</li>
 *  <li><b>[ALERT]</b> ALWAYS (success&failure) remember to destruct the returned array after use, by `delete[] obj_arr;`.</li>
 *  <li>[Note] "de-allocation" (i.e. ref cnt dec) of the Python-detection-func reference pointers <b>as params</b>
 *      is ALWAYS (success&failure) NOT committed</li>
 * </ul>
 * @param p_func_detect (param as reference) located detection function of the Python-online-tracker instance
 * @param img_buffer    (param as reference) allocated `char*` binary buffer instance, representing the target image
 * @param img_size_byte (param as copy) size of the image, in bytes
 * @return              a tuple of (char(*)[LEN_BBOX_FEATURE=4], int, int), where:
 *                          <ul>
 *                              <li><b>#1</b>: an allocated `char(*)[LEN_BBOX_FEATURE=4]` instance,
 *                                  representing a 2d-array shaped (cnt_obj, dim_feature),
 *                                  with features as (x_top_left,y_top_left,w,h).
 *                                  if successful, cnt_obj>=0; if otherwise, cnt_obj==0</li>
 *                              <li><b>#2</b>: cnt of objs detected, for 2d-array-iteration.
 *                                  if successful, value=cnt-array-rows; if otherwise, value<0 </li>
 *                              <li><b>#3</b>: dim of bbox features. is ALWAYS (success&failure) `LEN_BBOX_FEATURE`.</li>
 *                          </ul>
 */
static auto ExecDetect(PyObject *&p_func_detect, char *&img_buffer, int img_size_byte) {

    // reference:
    //  auto return type: https://stackoverflow.com/a/56059378
    //  variable-length array: https://stackoverflow.com/a/936702 -> https://stackoverflow.com/a/16239446

    // dim of the features representing a bounding-box
    static const int DIM_BBOX_FEATURE = 4;  // 4=x_top_left/y_top_left/w/h
    // local Python objects pointers
    PyObject *p_args, *p_return;
    PyObject *p_temp;

    // parse args
    int is_debug = 0;  // `0` if is TRUE, `1` otherwise
#ifdef DEBUG_UTILS_H
    is_debug = 1;
#endif
    p_args = Py_BuildValue("(y#,i)", img_buffer, img_size_byte, is_debug);  // will remain typed <int> in Python

    // call function
    p_return = PyObject_CallObject(p_func_detect, p_args);
    Py_DECREF(p_args);
    if (!p_return) {  // error handling
        delete[] img_buffer;
        PyErr_Print();
        fprintf(stderr, "[ERROR] Calling Detection Handler Failed\n");
        auto obj_arr_err = new int[0][DIM_BBOX_FEATURE];  // compatability alert: C++ 11 or up
        return std::make_tuple(obj_arr_err, -2, DIM_BBOX_FEATURE);  // compatability alert: C++ 11 or up
    }
    // return value check
    int obj_cnt = (int) PyList_Size(p_return);
    if (0 > obj_cnt) {
        delete[] img_buffer;
        PyErr_Print();
        fprintf(stderr, "[ERROR] Failed to Parse Call Result. Obj_Cnt < 0.\n");
        auto obj_arr_err = new int[0][DIM_BBOX_FEATURE];  // compatability alert: C++ 11 or up
        return std::make_tuple(obj_arr_err, -1, DIM_BBOX_FEATURE);  // compatability alert: C++ 11 or up
    }

    // handles the returned (N,LEN_BBOX_FEATURE) list
    auto obj_arr = new int[obj_cnt][DIM_BBOX_FEATURE];  // compatability alert: C++ 11 or up
    for (int _row_idx = 0; _row_idx <= obj_cnt - 1; _row_idx++) {
        p_temp = PyList_GetItem(p_return, _row_idx);
        for (int _col_idx = 0; _col_idx <= DIM_BBOX_FEATURE - 1; _col_idx++) {
            obj_arr[_row_idx][_col_idx] = (int) PyLong_AsLong(PyList_GetItem(p_temp, _col_idx));
        }
        Py_DECREF(p_temp);
    }
    Py_DECREF(p_return);

#ifdef DEBUG_UTILS_H
    printf("[DETECT] Result of call - (%d, %d) array of x_top_left/y_top_left/w/h:\n", obj_cnt, DIM_BBOX_FEATURE);
    if (0 >= obj_cnt) {
        std::cout << "\t===== NO DETECTED OBJECTS =====" << std::endl;
    }
    else {
        for (int _row_idx = 0; _row_idx <= obj_cnt - 1; _row_idx++) {
            std::cout << "\tobj #" << _row_idx << ": (" << std::flush;
            for (int _col_idx = 0; _col_idx <= DIM_BBOX_FEATURE - 1; _col_idx++) {
                std::cout << obj_arr[_row_idx][_col_idx] << "," << std::flush;
            }
            std::cout << ")" << std::endl;
        }
    }
    // view GT by:
    //  1. copy ~/test.jpg to ~/MOT/tracker/detector/YOLOv5/test.jpg
    //  2. cd to: ~/MOT/tracker/detector/YOLOv5/
    //  3. run: python detect.py --weights weight/best.pt --source test.jpg --save-txt --save-conf
    //  4. view bbox in: runs/detect/exp*/test.jpg
    //  5. view bbox values in: runs/detect/exp*/labels/test.txt
    //  6. parse & check:
    //      res given as: x, y, w, h
    //      GT given as: cls/x_normed/y_normed/w_normed/h_normed
#endif

    // destruct
    delete[] img_buffer;

    return std::make_tuple(obj_arr, obj_cnt, DIM_BBOX_FEATURE);  // compatability alert: C++ 11 or up;
}



// MOT - track (simulated): for template-alike deduced functions, impl in header files only
/**
 * for the previous obj-detected image, track all the objects.<br />
 * API notes:
 * <ul>
 *  <li> [Note] if successful, access elements in the return 2d-array `obj_arr`:
 *      call `int elem = obj_arr[bbox_idx][feature_idx];`.</li>
 *  <li><b>[ALERT]</b> ALWAYS (success&failure) remember to destruct the returned array after use, by `delete[] obj_arr;`.</li>
 *  <li>[Note] "de-allocation" (i.e. ref cnt dec) of the Python-detection-func reference pointers <b>as params</b>
 *      is ALWAYS (success&failure) NOT committed</li>
 * </ul>
 * @param p_func_track_sim  (param as reference) located tracking function of the Python-online-tracker instance
 * @return                  a tuple of (char(*)[DIM_TRACK_OBJ_FEATURE=5], int, int), where:
 *                          <ul>
 *                              <li><b>#1</b>: an allocated `char(*)[LEN_BBOX_FEATURE=4]` instance,
 *                                  representing a 2d-array shaped (cnt_obj, dim_feature),
 *                                  with features as (x_top_left,y_top_left,w,h).
 *                                  if successful, cnt_obj>=0; if otherwise, cnt_obj==0</li>
 *                              <li><b>#2</b>: cnt of objs detected, for 2d-array-iteration.
 *                                  if successful, value=cnt-array-rows; if otherwise, value<0 </li>
 *                              <li><b>#3</b>: dim of bbox features. is ALWAYS (success&failure) `DIM_TRACK_OBJ_FEATURE`.</li>
 *                          </ul>
 */
static auto ExecTrackSim(PyObject *&p_func_track_sim) {

    // reference:
    //  auto return type: https://stackoverflow.com/a/56059378
    //  variable-length array: https://stackoverflow.com/a/936702 -> https://stackoverflow.com/a/16239446

    // dim of the features representing a bounding-box
    static const int DIM_TRACK_OBJ_FEATURE = 5;  // 5=x_top_left/y_top_left/w/h/id
    // local Python objects pointers
    PyObject *p_args, *p_return;
    PyObject *p_temp;

    // parse args
    int is_debug = 0;  // `0` if is TRUE, `1` otherwise
#ifdef DEBUG_UTILS_H
    is_debug = 1;
#endif
    p_args = Py_BuildValue("(i)", is_debug);  // will remain typed <int> in Python

    // call function
    p_return = PyObject_CallObject(p_func_track_sim, p_args);
    Py_DECREF(p_args);
    if (!p_return) {  // error handling
        PyErr_Print();
        fprintf(stderr, "[ERROR] Calling Tracking Handler Failed\n");
        auto obj_arr_err = new int[0][DIM_TRACK_OBJ_FEATURE];  // compatability alert: C++ 11 or up
        return std::make_tuple(obj_arr_err, -2, DIM_TRACK_OBJ_FEATURE);  // compatability alert: C++ 11 or up
    }
    // return value check
    int obj_cnt = (int) PyList_Size(p_return);
    if (0 > obj_cnt) {
        PyErr_Print();
        fprintf(stderr, "[ERROR] Failed to Parse Call Result. Obj_Cnt < 0.\n");
        auto obj_arr_err = new int[0][DIM_TRACK_OBJ_FEATURE];  // compatability alert: C++ 11 or up
        return std::make_tuple(obj_arr_err, -1, DIM_TRACK_OBJ_FEATURE);  // compatability alert: C++ 11 or up
    }

    // handles the returned (N,LEN_BBOX_FEATURE) list
    auto obj_arr = new int[obj_cnt][DIM_TRACK_OBJ_FEATURE];  // compatability alert: C++ 11 or up
    for (int _row_idx = 0; _row_idx <= obj_cnt - 1; _row_idx++) {
        p_temp = PyList_GetItem(p_return, _row_idx);
        for (int _col_idx = 0; _col_idx <= DIM_TRACK_OBJ_FEATURE - 1; _col_idx++) {
            obj_arr[_row_idx][_col_idx] = (int) PyLong_AsLong(PyList_GetItem(p_temp, _col_idx));
        }
        Py_DECREF(p_temp);
    }
    Py_DECREF(p_return);

#ifdef DEBUG_UTILS_H
    printf("[TRACK] Result of call - (%d, %d) array of x_top_left/y_top_left/w/h/id:\n", obj_cnt,
           DIM_TRACK_OBJ_FEATURE);
    if (0 >= obj_cnt) {
        std::cout << "\t===== NO TRACKED OBJECTS =====" << std::endl;
    }
    else {
        for (int _row_idx = 0; _row_idx <= obj_cnt - 1; _row_idx++) {
            std::cout << "\tobj #" << _row_idx << ": (" << std::flush;
            for (int _col_idx = 0; _col_idx <= DIM_TRACK_OBJ_FEATURE - 1; _col_idx++) {
                std::cout << obj_arr[_row_idx][_col_idx] << "," << std::flush;
            }
            std::cout << ")" << std::endl;
        }
    }
#endif

    return std::make_tuple(obj_arr, obj_cnt, DIM_TRACK_OBJ_FEATURE);  // compatability alert: C++ 11 or up;
}


// MOT - track (spectrum): for template-alike deduced functions, impl in header files only
/**
 * for the previous obj-detected image, track all the objects using the spectrum image of all bbox (in the same order).<br />
 * API notes:
 * <ul>
 *  <li>[Note] destruction of the input buffer list instance (as param) will ALWAYS (success&failure) be committed</li>
 *  <li>[Note] destruction of the input size list instance (as param) will ALWAYS (success&failure) be committed</li>
 *  <li> [Note] if successful, access elements in the return 2d-array `obj_arr`:
 *      call `int elem = obj_arr[bbox_idx][feature_idx];`.</li>
 *  <li><b>[ALERT]</b> ALWAYS (success&failure) remember to destruct the returned array after use, by `delete[] obj_arr;`.</li>
 *  <li>[Note] "de-allocation" (i.e. ref cnt dec) of the Python-detection-func reference pointers <b>as params</b>
 *      is ALWAYS (success&failure) NOT committed</li>
 * </ul>
 * @param p_func_track_spectrum     (param as reference) located tracking function of the Python-online-tracker instance
 * @param detect_obj_cnt            (param as copy) number of detected objects.
 * @param img_buffer_list           (param as reference) allocated list instance of `char*` binary buffer instance,
 *                                      with each element representing the spectrum image of a bbox.
 *                                      Length of the list == param `detect_obj_cnt`.
 * @param img_size_byte_list        (param as reference) allocated list instance of `int` numbers,
 *                                      with each element representing the size of the spectrum image of a bbox, in bytes.
 *                                      Length of the list == param `detect_obj_cnt`.
 * @return                          a tuple of (char(*)[DIM_TRACK_OBJ_FEATURE=5], int, int), where:
 *                                  <ul>
 *                                      <li><b>#1</b>: an allocated `char(*)[LEN_BBOX_FEATURE=4]` instance,
 *                                          representing a 2d-array shaped (cnt_obj, dim_feature),
 *                                          with features as (x_top_left,y_top_left,w,h).
 *                                          if successful, cnt_obj>=0; if otherwise, cnt_obj==0</li>
 *                                  <li><b>#2</b>: cnt of objs detected, for 2d-array-iteration.
 *                                          if successful, value=cnt-array-rows; if otherwise, value<0 </li>
 *                                      <li><b>#3</b>: dim of bbox features. is ALWAYS (success&failure) `DIM_TRACK_OBJ_FEATURE`.</li>
 *                                  </ul>
 */
static auto ExecTrackSpectrum(PyObject *&p_func_track_spectrum, int detect_obj_cnt,
                              char **&img_buffer_list, int *&img_size_byte_list) {

    // dim of the features representing a bounding-box
    static const int DIM_TRACK_OBJ_FEATURE = 5;  // 5=x_top_left/y_top_left/w/h/id

    // local Python objects pointers
    PyObject *p_args, *p_return;
    PyObject *p_temp;

    // parse args
    int is_debug = 0;  // `0` if is TRUE, `1` otherwise
#ifdef DEBUG_UTILS_H
    is_debug = 1;
#endif
    p_temp = PyList_New(detect_obj_cnt);
    for (int _obj_idx = 0; _obj_idx <= detect_obj_cnt - 1; _obj_idx++) {
        char *_obj_img_buffer = img_buffer_list[_obj_idx];
        int _obj_img_size_byte = img_size_byte_list[_obj_idx];

        PyList_SetItem(p_temp, _obj_idx, Py_BuildValue("y#", _obj_img_buffer, _obj_img_size_byte));
    }
    p_args = PyTuple_New(2);
    PyTuple_SetItem(p_args, 0, p_temp);
    PyTuple_SetItem(p_args, 1, Py_BuildValue("i", is_debug));

    // call function
    p_return = PyObject_CallObject(p_func_track_spectrum, p_args);
    Py_DECREF(p_args);
    Py_DECREF(p_temp);
    if (!p_return) {  // error handling
        for (int i = 0; i <= detect_obj_cnt - 1; i++) {
            delete img_buffer_list[i];
        }
        delete[] img_buffer_list;
        delete[] img_size_byte_list;
        PyErr_Print();
        fprintf(stderr, "[ERROR] Calling Detection Handler Failed\n");
        auto obj_arr_err = new int[0][DIM_TRACK_OBJ_FEATURE];  // compatability alert: C++ 11 or up
        return std::make_tuple(obj_arr_err, -2, DIM_TRACK_OBJ_FEATURE);  // compatability alert: C++ 11 or up
    }

    // return value check
    int track_obj_cnt = (int) PyList_Size(p_return);
    if (0 > track_obj_cnt) {
        PyErr_Print();
        fprintf(stderr, "[ERROR] Failed to Parse Call Result. Obj_Cnt < 0.\n");
        auto obj_arr_err = new int[0][DIM_TRACK_OBJ_FEATURE];  // compatability alert: C++ 11 or up
        return std::make_tuple(obj_arr_err, -1, DIM_TRACK_OBJ_FEATURE);  // compatability alert: C++ 11 or up
    }

    // handles the returned (N,LEN_BBOX_FEATURE) list
    auto obj_arr = new int[track_obj_cnt][DIM_TRACK_OBJ_FEATURE];  // compatability alert: C++ 11 or up
    for (int _row_idx = 0; _row_idx <= track_obj_cnt - 1; _row_idx++) {
        p_temp = PyList_GetItem(p_return, _row_idx);
        for (int _col_idx = 0; _col_idx <= DIM_TRACK_OBJ_FEATURE - 1; _col_idx++) {
            obj_arr[_row_idx][_col_idx] = (int) PyLong_AsLong(PyList_GetItem(p_temp, _col_idx));
        }
        Py_DECREF(p_temp);
    }
    Py_DECREF(p_return);

#ifdef DEBUG_UTILS_H
    printf("[TRACK-SPECTRUM] Result of call - (%d, %d) array of x_top_left/y_top_left/w/h/id:\n",
           track_obj_cnt, DIM_TRACK_OBJ_FEATURE);
    if (0 >= track_obj_cnt) {
        std::cout << "\t===== NO TRACKED OBJECTS =====" << std::endl;
    }
    else {
        for (int _row_idx = 0; _row_idx <= track_obj_cnt - 1; _row_idx++) {
            std::cout << "\tobj #" << _row_idx << ": (" << std::flush;
            for (int _col_idx = 0; _col_idx <= DIM_TRACK_OBJ_FEATURE - 1; _col_idx++) {
                std::cout << obj_arr[_row_idx][_col_idx] << "," << std::flush;
            }
            std::cout << ")" << std::endl;
        }
    }
#endif

    return std::make_tuple(obj_arr, track_obj_cnt, DIM_TRACK_OBJ_FEATURE);  // compatability alert: C++ 11 or up;
}

// inputs
template<typename ... Args>
std::string string_format(const std::string &format, Args ... args) {
    int size_s = std::snprintf(nullptr, 0, format.c_str(), args ...) + 1; // Extra space for '\0'
    if (size_s <= 0) { throw std::runtime_error("Error during formatting."); }
    auto size = static_cast<size_t>( size_s );
    std::unique_ptr<char[]> buf(new char[size]);
    std::snprintf(buf.get(), size, format.c_str(), args ...);
    return {buf.get(), buf.get() + size - 1}; // We don't want the '\0' inside
}

std::tuple<char *, int> MakeImageStreamBufferFromFile();  // compatability alert: C++ 11 or up
std::tuple<char *, int> MakeImageStreamBufferFromFile(
        const std::string &img_path);  // compatability alert: C++ 11 or up
#endif //INC_202302_SPECTRUM_MOT_UTILS_H
