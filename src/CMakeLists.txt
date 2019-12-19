cmake_minimum_required(VERSION 3.10)

project(InSituVTKWriters)

set (CMAKE_CXX_STANDARD 11)
include_directories(${CMAKE_SOURCE_DIR}/InWrap)
include_directories(${CMAKE_SOURCE_DIR}/InWrap/utils)
include_directories(${CMAKE_SOURCE_DIR}/InWrap/vtkStructures)
#include_directories(${CMAKE_SOURCE_DIR}/testing)


# link shared lib with full rpath
set (CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
set (CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

# setup cache variables for ccmake
if (NOT CMAKE_BUILD_TYPE)
    set (CMAKE_BUILD_TYPE RelWithDebInfo
         CACHE STRING "Choose the type of build." FORCE)
    set_property (CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS 
                  "Debug" "Release" "RelWithDebInfo" "MinSizeRel")
endif ()

set (CMAKE_PREFIX_PATH "" CACHE STRING "External dependencies path")
set (BUILD_SHARED_LIBS "OFF" CACHE BOOL "Build a shared library")


# Catalyst
option(ENABLE_CATALYST "Link the sim with Catalyst for in situ" OFF)
if (ENABLE_CATALYST)
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
endif()


# InSitu
option(ENABLE_INSITU "Enable InSitu" ON)
if (ENABLE_INSITU)
	add_definitions(-DINSITU_ON)


	# VeloC
	option(ENABLE_VELOC "Link the sim with SENSEI for in situ" OFF)
	if (ENABLE_VELOC)
		set (VELOC_INCLUDE_PATH ${VELOC_INCLUDE_PATH} CACHE PATH "VeloC include directory")
		set (VELOC_CLIENT_LIB ${VELOC_CLIENT_LIB} CACHE PATH "VeloC client library")

		add_definitions(-DVELOC_ENABLED)
	endif()


	# Sensei
	option(ENABLE_SENSEI "Link the sim with SENSEI for in situ" OFF)
	if (ENABLE_SENSEI)
		find_package(SENSEI REQUIRED)

		if (NOT SENSEI_FOUND)
			message(FATAL_ERROR "Sensei not found")
		else()
			add_definitions(-DSENSEI_ENABLED)
			set(SENSEI_ENABLED ON)
		endif()
	endif()

endif()


# PAPI
option(USE_PAPI "Use PAPI for profiling" OFF)
if (USE_PAPI)
	set (PAPI_INCLUDE_PATH ${PAPI_INCLUDE_PATH} CACHE PATH "Papi include directory")
  	set (PAPI_LIBRARY ${PAPI_LIBRARY} CACHE PATH "Papi library")
  	include_directories(${PAPI_INCLUDE_PATH})

  	add_definitions(-DPAPI_ENABLED)
endif()


# Tau
option(USE_TAU "Use TAU for profiling" OFF)
if (USE_TAU)
	set (CMAKE_CXX_COMPILER tau_cxx.sh)
	set (CMAKE_C_COMPILER tau_cc.sh)
	add_definitions(-DTAU_ENABLED)
endif()


# Mochi
option(USE_MOCHI "Enable Mochi" OFF)
if (USE_MOCHI)
	find_package(PkgConfig REQUIRED)

	# add our cmake module directory to the path
	set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
     			"${CMAKE_CURRENT_SOURCE_DIR}/InWrap/cmake")

	# packages we depend on
	include (xpkg-import)
	find_package (mercury CONFIG REQUIRED)
	include_directories(${MERCURY_INCLUDE_DIR})
	xpkg_import_module (argobots REQUIRED argobots)
	xpkg_import_module (margo REQUIRED margo)

	add_definitions(-DMOCHI_ENABLED)
	set(MOCHI_ENABLED ON)
endif()


## Not Optional!!!

# VTK
if (NOT HAVE_VTK)
	find_package(VTK REQUIRED)
	if (VTK_FOUND)
		include(${VTK_USE_FILE})
		add_definitions(-DHAVE_VTK)
	else()
		message(FATAL_ERROR "Could not find vtk!!!") 
	endif()
endif()


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


# Applications
#add_subdirectory(testing)
add_subdirectory(demoApps)


# Prevent in source builds since they are evil
if (CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
	message(FATAL_ERROR "In source build is not supported, please restore modified files, 
						delete CMake generated things (CMakeFiles folder and CMakeCache.txt)
						, and build out of source")	
endif()



## Install

# Create include folder for install
file(GLOB subdir "${CMAKE_SOURCE_DIR}/utils/*")
foreach (file ${subdir})
	install(FILES ${file} DESTINATION include)
endforeach()

file(GLOB subdir "${CMAKE_SOURCE_DIR}/vtkStructures/*")
foreach (file ${subdir})
	install(FILES ${file} DESTINATION include)
endforeach()

install(FILES inSitu/inSituAdaptor.h DESTINATION include)
install(FILES inSitu/catalystAdaptor.h DESTINATION include)


# Remove output files
add_custom_target(clean-output
	COMMAND rm -f *.vtu
	COMMAND rm -f *.pvtu
	COMMAND rm -f *.vts
	COMMAND rm -f *.pvts
)