#include "python_frame.h"
#include <sstream>


const static size_t MAX_NUM_FRAMES = 30;
thread_local static python_frame_t python_frames[MAX_NUM_FRAMES];

std::string get_frames() {
    size_t num_frames = 0;

    python_frame_get(MAX_NUM_FRAMES, python_frames, &num_frames);

    std::stringstream ss;

    for (size_t i = 0; i < num_frames; i++) {
        ss << "f-" << std::to_string(i) << "  "
           << std::string(python_frames[i].file_name) << ":"
           << std::to_string(python_frames[i].function_first_lineno) << "  def "
           << std::string(python_frames[i].function_name) << "():"
           << std::to_string(python_frames[i].lineno)
           << std::endl;
    }
    // std::cout << ss.str() << std::endl;
    return ss.str();
}

PYBIND11_MODULE(pyanalyzer, m) {
    m.doc() = "A tool for python code reviewing!"; // optional module docstring

    m.def("get_frames", &get_frames, "A function which returns the current python frames");
}