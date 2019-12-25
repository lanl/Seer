cmake_minimum_required(VERSION 3.10)
project(mymath)

find_package(PythonLibs REQUIRED)
find_package(Python 3 REQUIRED)

# Numpy
find_package(NumPy)





execute_process(
            COMMAND ${PYTHON_EXECUTABLE} -c "import numpy as n\nprint n.get_include()"
            OUTPUT_VARIABLE numpy_include
            OUTPUT_STRIP_TRAILING_WHITESPACE
            )

include_directories(${PYTHON_INCLUDE_DIRS} ${numpy_include})       

message(${PYTHON_INCLUDE_DIRS} )
#message(${numpy_include})


add_executable(test testPy.cpp)
target_link_libraries(test ${PYTHON_LIBRARIES})