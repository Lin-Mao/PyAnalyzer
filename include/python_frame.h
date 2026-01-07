#ifndef PYTHON_FRAME_H
#define PYTHON_FRAME_H

#include <Python.h>
#include <pybind11/pybind11.h>

#include <string>
#include <vector>

typedef struct PythonFrame {
    std::string file_name;
    std::string func_name;
    size_t func_first_lineno;
    size_t lineno;

    PythonFrame(const std::string &file_name, const std::string &func_name,
                size_t func_first_lineno, size_t lineno)
            : file_name(file_name),
            func_name(func_name),
            func_first_lineno(func_first_lineno),
            lineno(lineno) {}
}PythonFrame_t;

class PyFrameChecker {
public:
    // Return the current python frames with a query or using the previous cached frames
    std::vector<PythonFrame_t> &get_frames(bool cached = false);

    // Get the singleton instance
    static PyFrameChecker &instance();

private:
    PyFrameChecker() {}

    std::string unpack_pyobject(PyObject *obj);

private:
    // Cached frames for each thread
    static inline thread_local std::vector<PythonFrame_t> _frames;
};

bool get_python_frame(std::vector<PythonFrame_t> &frames);

#endif  // PYTHON_FRAME_H