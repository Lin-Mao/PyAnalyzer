#include "python_frame.h"
#include <sstream>

static int count = 0;

std::string get_frames() {
    std::vector<PythonFrame_t> python_frames;
    get_python_frame(python_frames);

    std::stringstream ss;
    ss << "PyAnalyzer Frame Index: " << count++ << std::endl;
    for (size_t i = 0; i < python_frames.size(); i++) {
        ss << "f-" << std::to_string(i) << " "
           << std::string(python_frames[i].file_name) << ":"
           << std::to_string(python_frames[i].lineno) << "  def "
           << std::string(python_frames[i].func_name) << "() "
           << std::string(python_frames[i].file_name) << ":"
           << std::to_string(python_frames[i].func_first_lineno)
           << std::endl;
    }
    // std::cout << ss.str() << std::endl;
    return ss.str();
}

PYBIND11_MODULE(pyanalyzer, m) {
    m.doc() = "A tool for python code reviewing!"; // optional module docstring

    m.def("get_frames", &get_frames, "A function which returns the current python frames");
}