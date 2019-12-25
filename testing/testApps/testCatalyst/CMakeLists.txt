cmake_minimum_required(VERSION 3.10)

project(TestCatalyst)


# Catalyst
find_package(ParaView REQUIRED COMPONENTS vtkPVPythonCatalyst)

if (NOT ParaView_FOUND)
	message(FATAL_ERROR "Catalyst analysis components require Catalyst build"
		"(or install directory. Please set ParaView_DIR to point to " "directory"
		"containing `ParaViewConfig.cmake`.")
endif()

if (NOT PARAVIEW_USE_MPI)
	message(SEND_ERROR "ParaView must be built with MPI enabled")
endif()

include("${PARAVIEW_USE_FILE}")

set(CATALYST_ENABLED ON)

include ( "${ParaView_DIR}/VTKConfig.cmake" )
add_definitions(-DHAVE_VTK)
set(HAVE_VTK ON)

set(VTK_FOUND ON CACHE BOOL "Link to VTK")
set(VTK_INCLUDED ON CACHE BOOL "Link to VTK")

add_definitions(-DCATALYST_ENABLED)



# MPI
find_package(MPI)
if (MPI_FOUND)
	include_directories(${MPI_INCLUDE_PATH})
	set(CMAKE_CXX_COMPILE_FLAGS ${CMAKE_CXX_COMPILE_FLAGS} ${MPI_COMPILE_FLAGS})
	set(CMAKE_CXX_LINK_FLAGS ${CMAKE_CXX_LINK_FLAGS} ${MPI_LINK_FLAGS})
	SET(CMAKE_C_COMPILER mpicc)
	SET(CMAKE_CXX_COMPILER mpicxx)
else()
	message(FATAL_ERROR "Could not find any MPI installation!!!") 
endif()


#
add_executable(testCatalystSim testSim.cpp)
target_link_libraries(testCatalystSim ${MPI_LIBRARIES} ${VTK_LIBRARIES} vtkPVPythonCatalyst vtkParallelMPI)