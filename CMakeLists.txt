cmake_minimum_required(VERSION 3.10)

project(Seer VERSION "0.3")

set (CMAKE_CXX_STANDARD 11)
include_directories(${CMAKE_SOURCE_DIR}/Seer_Sim)
include_directories(${CMAKE_SOURCE_DIR}/Seer_Sim/utils)
include_directories(${CMAKE_SOURCE_DIR}/Seer_Sim/vtkStructures)
include_directories(${CMAKE_SOURCE_DIR}/Seer_Utils)

set (SEER_LIBS "")


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
  	find_package(ParaView REQUIRED)

  	if (NOT TARGET ParaView::PythonCatalyst)
    	message(STATUS
      		"Skipping example: ${CMAKE_PROJECT_NAME} requires ParaView to be built "
      		"with Catalyst and Python support enabled. Please rebuild ParaView (or "
      		"point to a different build of ParaView) with PARAVIEW_USE_PYTHON set "
      		"to TRUE")
    	return ()
 	 endif()

  	if (NOT PARAVIEW_USE_MPI)
    	message(STATUS
      		"Skipping example: ${CMAKE_PROJECT_NAME} requires ParaView to be built "
      		"with MPI support enabled. Please rebuild ParaView (or point to a "
      		"different build of ParaView) with PARAVIEW_USE_MPI set to TRUE")
    	return ()
  	endif ()

  	message(STATUS "Found Paraview: ${ParaView_DIR}")
  	message(STATUS "IO with Paraview Catalyst enabled" )


  	list( APPEND INWRAP_LIBS ParaView::PythonCatalyst VTK::CommonDataModel VTK::ParallelMPI VTK::IOParallelXML)

	add_definitions(-DCATALYST_ENABLED)
endif()


# VeloC - NOT Supported right now!
option(ENABLE_VELOC "Link the sim with SENSEI for in situ" OFF)
if (ENABLE_VELOC)
	set (VELOC_INCLUDE_PATH ${VELOC_INCLUDE_PATH} CACHE PATH "VeloC include directory")
	set (VELOC_CLIENT_LIB ${VELOC_CLIENT_LIB} CACHE PATH "VeloC client library")

	add_definitions(-DVELOC_ENABLED)
endif()


# Sensei - NOT Supported right now!
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




# PAPI
option(USE_PAPI "Use PAPI for profiling" OFF)
if (USE_PAPI)
	set (PAPI_INCLUDE_PATH ${PAPI_INCLUDE_PATH} CACHE PATH "Papi include directory")
  	set (PAPI_LIBRARY ${PAPI_LIBRARY} CACHE PATH "Papi library")
  	include_directories(${PAPI_INCLUDE_PATH})
    message(${PAPI_INCLUDE_PATH})
  	add_definitions(-DPAPI_ENABLED)
endif()



## Not Optional!!!

# Mochi
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

# OpenMP
find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
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
