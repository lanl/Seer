cmake_minimum_required (VERSION 3.0)
project (TestThallium)

include_directories(${CMAKE_SOURCE_DIR}/utils)
include_directories(${CMAKE_SOURCE_DIR}/client)

find_package(MPI REQUIRED)

find_package(mercury REQUIRED)
find_package(thallium REQUIRED)

add_subdirectory(server)

add_executable(client test3.cpp)
target_link_libraries(client thallium MPI::MPI_CXX)