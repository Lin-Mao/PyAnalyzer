cmake_minimum_required(VERSION 3.16.3)
project(PyAnalyzer)

set(CMAKE_CXX_STANDARD 17)

file(GLOB PYANALYZER_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

# python
find_package(Python COMPONENTS Interpreter Development)
if (NOT ${Python_FOUND})
    message(SEND_ERROR "PYTHON not found")
endif()
message(STATUS "PYTHON found: ${Python_FOUND}")
message(STATUS "PYTHON include: ${Python_INCLUDE_DIRS}")
message(STATUS "PYTHON libraries: ${Python_LIBRARIES}")

set(PYBIND_DIR ${CMAKE_CURRENT_SOURCE_DIR}/pybind11)
add_subdirectory(${PYBIND_DIR})
pybind11_add_module(pyanalyzer ${PYANALYZER_SOURCE})

target_include_directories(pyanalyzer PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include
                            ${Python_INCLUDE_DIRS})
target_link_libraries(pyanalyzer PRIVATE ${Python_LIBRARIES})