cmake_minimum_required (VERSION 3.0)
project (TestThallium)

find_package(MPI REQUIRED)

find_package(mercury REQUIRED)
find_package(thallium REQUIRED)

add_executable(thallium_client client.cpp)
target_link_libraries(thallium_client thallium MPI::MPI_CXX)

add_executable(thallium_server server.cpp)
target_link_libraries(thallium_server thallium MPI::MPI_CXX)
