#include "python_frame.h"

PyFrameChecker& PyFrameChecker::instance() {
    static PyFrameChecker monitor;
    return monitor;
}

// Take from PyTorch::THPUtils_unpackStringView
std::string PyFrameChecker::unpack_pyobject(PyObject* obj) {
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

std::vector<PythonFrame_t>& PyFrameChecker::get_frames(bool cached) {
    if (cached) {
        return _frames;
    }

    // GIL lock is required
    pybind11::gil_scoped_acquire gil;

    PyFrameObject* frame = PyEval_GetFrame();
    _frames.clear();

    while (nullptr != frame) {
        size_t lineno = PyFrame_GetLineNumber(frame);
        size_t func_first_lineno = frame->f_code->co_firstlineno;
        std::string file_name = unpack_pyobject(frame->f_code->co_filename);
        std::string func_name = unpack_pyobject(frame->f_code->co_name);
        _frames.emplace_back(PythonFrame_t{file_name, func_name, func_first_lineno, lineno});
        frame = frame->f_back;
    }
    return _frames;
}

bool get_python_frame(std::vector<PythonFrame_t> &frames) {
    auto &frame_checker = PyFrameChecker::instance();
    frames = frame_checker.get_frames();    

    return frames.size() > 0;
}