#ifndef PYTHON_FRAME_H
#define PYTHON_FRAME_H

#include <Python.h>
#include <pybind11/pybind11.h>

#include <string>
#include <vector>

typedef struct python_frame {
    const char *file_name;
    const char *function_name;
    size_t function_first_lineno;
    size_t lineno;
}python_frame_t;

struct PythonFrame {
    std::string file_name;
    std::string function_name;
    size_t function_first_lineno;
    size_t lineno;

    PythonFrame(const std::string &file_name, const std::string &function_name,
                size_t function_first_lineno, size_t lineno)
            : file_name(file_name),
            function_name(function_name),
            function_first_lineno(function_first_lineno),
            lineno(lineno) {}
};

class PythonFrameMonitor {
public:
    // Return the current python frames with a query or using the previous cached frames
    std::vector<PythonFrame> &get_frames(bool cached = false);

    // Get the singleton instance
    static PythonFrameMonitor &instance();

private:
    PythonFrameMonitor() {}

    std::string unpack_pyobject(PyObject *obj);

private:
    // Cached frames for each thread
    static inline thread_local std::vector<PythonFrame> _frames;
};

bool python_frame_get(size_t max_num_frames, python_frame_t *frames, size_t *num_frames);

#endif  // PYTHON_FRAME_H