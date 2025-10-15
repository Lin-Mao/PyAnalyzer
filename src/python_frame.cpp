#include "python_frame.h"
#include <frameobject.h>  // PyFrame_* / PyCode_* APIs

// Backport for < 3.10
#if PY_VERSION_HEX < 0x030A0000
static inline PyObject* Py_NewRef(PyObject* obj) { Py_INCREF(obj); return obj; }
static inline PyObject* Py_XNewRef(PyObject* obj) { Py_XINCREF(obj); return obj; }
#endif


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
    if (cached) return _frames;

    pybind11::gil_scoped_acquire gil;  // you already noted GIL is required
    _frames.clear();

    // Own a reference to the current frame
    PyThreadState* tstate = PyThreadState_Get();
    PyFrameObject* frame =
        (PyFrameObject*)Py_XNewRef((PyObject*)PyThreadState_GetFrame(tstate));  // may be NULL

    while (frame) {
        size_t lineno = (size_t)PyFrame_GetLineNumber(frame);

        #if PY_VERSION_HEX >= 0x030B0000
            // PyFrameObject is opaque in 3.11+, use accessors
            PyCodeObject* code = PyFrame_GetCode(frame);  // new ref (3.11+)
            size_t func_first_lineno = 0;
            std::string file_name, func_name;
            if (code) {
                PyObject* co_filename = PyObject_GetAttrString((PyObject*)code, "co_filename");
                PyObject* co_name = PyObject_GetAttrString((PyObject*)code, "co_name");
                PyObject* co_firstlineno = PyObject_GetAttrString((PyObject*)code, "co_firstlineno");
                file_name = co_filename ? unpack_pyobject(co_filename) : "";
                func_name = co_name ? unpack_pyobject(co_name) : "";
                if (co_firstlineno && PyLong_Check(co_firstlineno)) {
                    func_first_lineno = (size_t)PyLong_AsUnsignedLong(co_firstlineno);
                }
                Py_XDECREF(co_filename);
                Py_XDECREF(co_name);
                Py_XDECREF(co_firstlineno);
                Py_DECREF(code);
            }
            _frames.emplace_back(PythonFrame_t{file_name, func_name, func_first_lineno, lineno});
            frame = PyFrame_GetBack(frame);  // borrowed; do not DECREF
        #else
            // ≤3.10: direct field access still allowed
            size_t func_first_lineno = (size_t)frame->f_code->co_firstlineno;
            std::string file_name = unpack_pyobject(frame->f_code->co_filename);
            std::string func_name = unpack_pyobject(frame->f_code->co_name);
            _frames.emplace_back(PythonFrame_t{file_name, func_name, func_first_lineno, lineno});
            frame = frame->f_back;
        #endif
    }

    return _frames;
}


bool get_python_frame(std::vector<PythonFrame_t> &frames) {
    auto &frame_checker = PyFrameChecker::instance();
    frames = frame_checker.get_frames();    

    return frames.size() > 0;
}