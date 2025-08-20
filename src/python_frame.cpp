#include "python_frame.h"
#include <frameobject.h>  // PyFrame_* / PyCode_* APIs


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
        (PyFrameObject*)Py_XNewRef(PyThreadState_GetFrame(tstate));  // may be NULL

    while (frame) {
        // Current executing line
        int lineno = PyFrame_GetLineNumber(frame);

        // Get code object (new ref)
        PyCodeObject* code = PyFrame_GetCode(frame);

        // Defaults
        long firstlineno = 0;
        std::string file_name, func_name;

        if (code) {
            // co_firstlineno (int)
            PyObject* first_obj =
                PyObject_GetAttrString((PyObject*)code, "co_firstlineno");
            if (first_obj) {
                long v = PyLong_AsLong(first_obj);
                if (!PyErr_Occurred() && v >= 0) firstlineno = v;
                Py_DECREF(first_obj);
            }

            // co_filename (str) and co_name (str)
            PyObject* filename_obj =
                PyObject_GetAttrString((PyObject*)code, "co_filename");
            PyObject* name_obj =
                PyObject_GetAttrString((PyObject*)code, "co_name");

            if (filename_obj) {
                file_name = unpack_pyobject(filename_obj);  // your helper
                Py_DECREF(filename_obj);
            }
            if (name_obj) {
                func_name = unpack_pyobject(name_obj);      // your helper
                Py_DECREF(name_obj);
            }

            Py_DECREF(code);
        }

        _frames.emplace_back(PythonFrame_t{
            file_name,
            func_name,
            static_cast<size_t>(firstlineno),
            static_cast<size_t>(lineno >= 0 ? lineno : 0)
        });

        // Walk to caller (own next before dropping current)
        PyFrameObject* next = (PyFrameObject*)Py_XNewRef(PyFrame_GetBack(frame));
        Py_DECREF(frame);
        frame = next;
    }

    return _frames;
}


bool get_python_frame(std::vector<PythonFrame_t> &frames) {
    auto &frame_checker = PyFrameChecker::instance();
    frames = frame_checker.get_frames();    

    return frames.size() > 0;
}