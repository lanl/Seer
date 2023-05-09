add_executable(thallium_server server.cpp)
target_link_libraries(thallium_server thallium MPI::MPI_CXX)