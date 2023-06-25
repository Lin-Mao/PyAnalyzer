#include "python_frame.h"

PythonFrameMonitor& PythonFrameMonitor::instance() {
    static PythonFrameMonitor monitor;
    return monitor;
}

// Take from PyTorch::THPUtils_unpackStringView
std::string PythonFrameMonitor::unpack_pyobject(PyObject* obj) {
    if (PyBytes_Check(obj)) {
        size_t size = PyBytes_GET_SIZE(obj);
        return std::string(PyBytes_AS_STRING(obj), size);
    }
    if (PyUnicode_Check(obj)) {
        // NOLINTNEXTLINE(cppcoreguidelines-init-variables)
        Py_ssize_t size;
        const char* data = PyUnicode_AsUTF8AndSize(obj, &size);
        if (!data) {
            // If we get any runtime error, just return an empty string to continue running
            // printf("obj %p utf8 parsing error", obj);
            return "";
        }
        return std::string(data, (size_t)size);
    }
    // printf("obj %p not bytes or unicode", obj);
    return "";
}

std::vector<PythonFrame>& PythonFrameMonitor::get_frames(bool cached) {
    if (cached) {
        return _frames;
    }

    // GIL lock is required
    pybind11::gil_scoped_acquire gil;

    PyFrameObject* frame = PyEval_GetFrame();
    _frames.clear();

    while (nullptr != frame) {
        size_t lineno = PyFrame_GetLineNumber(frame);
        size_t co_firstlineno = frame->f_code->co_firstlineno;
        std::string file_name = unpack_pyobject(frame->f_code->co_filename);
        std::string function_name = unpack_pyobject(frame->f_code->co_name);
        _frames.emplace_back(PythonFrame{file_name, function_name, co_firstlineno, lineno});
        frame = frame->f_back;
    }
    return _frames;
}

bool python_frame_get(size_t max_num_frames, python_frame_t *frames, size_t *num_frames) {

    bool status = true;

    auto &python_frame_monitor = PythonFrameMonitor::instance();

    auto &python_frames = python_frame_monitor.get_frames();

    if (python_frames.empty()) {
        status = false;
    } else {
        status = true;

        *num_frames = std::min(python_frames.size(), max_num_frames);
        for (size_t i = 0; i < *num_frames; ++i) {
            frames[i].file_name = python_frames[i].file_name.c_str();
            frames[i].function_name = python_frames[i].function_name.c_str();
            frames[i].function_first_lineno = python_frames[i].function_first_lineno;
            frames[i].lineno = python_frames[i].lineno;
        }
    }

  return status;
}