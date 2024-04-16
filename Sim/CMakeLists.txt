project(Seer)

cmake_minimum_required(VERSION 3.10)


# MPI
find_package(MPI REQUIRED)

# Mochi
find_package(mercury REQUIRED)
find_package(thallium REQUIRED)
find_package(yokan REQUIRED)
find_package(bedrock REQUIRED)

# JSON
find_package(nlohmann_json REQUIRED)

# BLOSC
#find_package(c-blosc2 REQUIRED) # not working

# Manually ask for library and include dir
find_library(BLOSC_LIBRARY ${BLOSC_LIBRARY} CACHE STRING "BLOSC_LIBRARY library")
set (BLOSC_INCLUDE_PATH ${BLOSC_INCLUDE_PATH} CACHE PATH "BLOSC include directory")
include_directories(${BLOSC_INCLUDE_PATH})

add_executable(sim_test testSim.cpp)
target_link_libraries(sim_test PUBLIC MPI::MPI_CXX yokan-client yokan-server nlohmann_json::nlohmann_json ${BLOSC_LIBRARY})

#/vast/home/pascalgrosset/spack/opt/spack/linux-rhel8-haswell/gcc-9.4.0/c-blosc2-2.11.1-rkb63dt4yt5xzwamtpdhixbvbmgbwskm/lib64/cmake/Blosc2