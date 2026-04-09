#include "python_frame.h"
#include <sstream>
#include <iostream>
#include <string>
#include <set>

static int count = 0;
static std::set<std::string> unique_frames;

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

void unique_show() {
    std::vector<PythonFrame_t> python_frames;
    get_python_frame(python_frames);

    std::cout << std::flush;
    std::stringstream ss;
    for (size_t i = 0; i < python_frames.size(); i++) {
        ss << "f-" << std::to_string(i) << " "
           << std::string(python_frames[i].file_name) << ":"
           << std::to_string(python_frames[i].lineno) << "  def "
           << std::string(python_frames[i].func_name) << "() "
           << std::endl;
    }
    // std::cout << ss.str() << std::endl;
    if (unique_frames.find(ss.str()) == unique_frames.end()) {
        unique_frames.insert(ss.str());
        std::cout << "=============== Unique Python Frame Index: " << count++ << " ===============\n";
        std::cout << ss.str() << std::endl;
        std::cout << std::flush;
    }
}


void show(int max_count) {
    if (count > max_count) {
        return ;
    }

    std::vector<PythonFrame_t> python_frames;
    get_python_frame(python_frames);

    std::cout << std::flush;
    std::stringstream ss;
    ss << "=============== Python Frame Index: " << count++ << " ===============\n";
    for (size_t i = 0; i < python_frames.size(); i++) {
        ss << "f-" << std::to_string(i) << " "
           << std::string(python_frames[i].file_name) << ":"
           << std::to_string(python_frames[i].lineno) << "  def "
           << std::string(python_frames[i].func_name) << "() "
           << std::endl;
    }
    std::cout << ss.str() << std::endl;
    std::cout << std::flush;
}


void show(const std::string& str) {
    std::vector<PythonFrame_t> python_frames;
    get_python_frame(python_frames);

    std::cout << std::flush;
    std::stringstream ss;
    ss << "=============== " << str << " ===============\n";
    for (size_t i = 0; i < python_frames.size(); i++) {
        ss << "f-" << std::to_string(i) << " "
           << std::string(python_frames[i].file_name) << ":"
           << std::to_string(python_frames[i].lineno) << "  def "
           << std::string(python_frames[i].func_name) << "() "
           << std::endl;
    }
    std::cout << ss.str() << std::endl;
    std::cout << std::flush;
}


PYBIND11_MODULE(pyanalyzer, m) {
    m.doc() = "A tool for python code reviewing!"; // optional module docstring

    m.def("get_frames", &get_frames, "A function which returns the current python frames");

    m.def("show",
          pybind11::overload_cast<int>(&show),
          "A function which shows the current python frames",
          pybind11::arg("max_count") = 2147483647);

    m.def("show",
          pybind11::overload_cast<const std::string&>(&show),
          "A function which shows the current python frames",
          pybind11::arg("str"));

    m.def("unique_show", &unique_show,
          "A function which shows the current python frames without duplicates");
}
