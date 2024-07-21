cmake_minimum_required(VERSION 3.10)
project(Seer)


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


add_library(seer_o STATIC
        src/seer_o.cpp
)

#add_executable(sim_test testSim.cpp)
target_link_libraries(seer_o PUBLIC nlohmann_json::nlohmann_json ${BLOSC_LIBRARY} MPI::MPI_CXX yokan-client yokan-server  )
target_include_directories(seer_o PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)


set(CMAKE_BUILD_TYPE debug)


install(TARGETS seer_o
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

install(DIRECTORY src/ DESTINATION include
                FILES_MATCHING PATTERN "*")