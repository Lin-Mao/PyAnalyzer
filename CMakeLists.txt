cmake_minimum_required(VERSION 3.16.3)
project(PySpy)

set(CMAKE_CXX_STANDARD 17)

file(GLOB PYSPY_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

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
pybind11_add_module(pyspy ${PYSPY_SOURCE})

target_include_directories(pyspy PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include
                            ${Python_INCLUDE_DIRS})
target_link_libraries(pyspy PRIVATE ${Python_LIBRARIES})

install(TARGETS pyspy DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/bin)
