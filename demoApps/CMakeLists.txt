# Catalyst
if (CATALYST_ENABLED)
	message("CATALYST_ENABLED")
	add_executable(miniAppUnstruc miniAppUnstruc.cpp)
	add_executable(miniAppStructured miniAppStructured.cpp)

	if (MOCHI_ENABLED)
		message("MOCHI_ENABLED")
		target_link_libraries(miniAppStructured ${MPI_LIBRARIES} ${VTK_LIBRARIES} papi vtkPVPythonCatalyst vtkParallelMPI margo sdskv-client)
		target_link_libraries(miniAppUnstruc ${MPI_LIBRARIES} ${VTK_LIBRARIES} papi vtkPVPythonCatalyst vtkParallelMPI margo sdskv-client)
	else()
		message("MOCHI_NOT_ENABLED")
		target_link_libraries(miniAppStructured ${MPI_LIBRARIES} ${VTK_LIBRARIES} papi vtkPVPythonCatalyst vtkParallelMPI )
		target_link_libraries(miniAppUnstruc ${MPI_LIBRARIES} ${VTK_LIBRARIES} papi vtkPVPythonCatalyst vtkParallelMPI )
	endif()

	include(vtkModuleMacros)
  	include(vtkMPI)
  
elseif (SENSEI_ENABLED)
	message("SENSEI_ENABLED")

	if (MOCHI_ENABLED)
		message("MOCHI_ENABLED")
		target_link_libraries(miniAppStructured ${MPI_LIBRARIES} ${VTK_LIBRARIES} papi margo sdskv-client sensei senseiCore)
		target_link_libraries(miniAppUnstruc ${MPI_LIBRARIES} ${VTK_LIBRARIES} papi margo sdskv-client sensei senseiCore)
	else()
		message("MOCHI_NOT_ENABLED")
		target_link_libraries(miniAppStructured ${MPI_LIBRARIES} ${VTK_LIBRARIES} papi sensei senseiCore)
		target_link_libraries(miniAppUnstruc ${MPI_LIBRARIES} ${VTK_LIBRARIES} papi sensei senseiCore)
	endif()

	include(vtkModuleMacros)
  	include(vtkMPI)

else()
	add_executable(miniAppUnstruc miniAppUnstruc.cpp)
	add_executable(miniAppStructured miniAppStructured.cpp)

	if (MOCHI_ENABLED)
		message("+MOCHI_ENABLED")
		target_link_libraries(miniAppStructured ${MPI_LIBRARIES} ${VTK_LIBRARIES} papi margo sdskv-client)
		target_link_libraries(miniAppUnstruc ${MPI_LIBRARIES} ${VTK_LIBRARIES} papi margo sdskv-client)
	else()
		message("-MOCHI_NOT_ENABLED")
		target_link_libraries(miniAppStructured ${MPI_LIBRARIES} ${VTK_LIBRARIES} papi)
		target_link_libraries(miniAppUnstruc ${MPI_LIBRARIES} ${VTK_LIBRARIES} papi)
	endif()

	
endif()


#if (USE_MOCHI)
#	message("MOCHI_ENABLED_MPI")
#	add_executable(testMochiMPI mpiTest.cpp)
#	target_include_directories(testMochiMPI PRIVATE ${MPI_CXX_INCLUDE_PATH})
#	target_include_directories(testMochiMPI PRIVATE ${/home/pascal/projects/sds-keyval/include})
#	target_link_libraries(testMochiMPI ${MPI_CXX_LIBRARIES} ${MPI_CXX_LINK_FLAGS})
#	target_link_libraries(testMochiMPI sdskv-client)
#	target_link_libraries(testMochiMPI margo)
#endif()


# Remove output files
add_custom_target(clean-demo
	COMMAND rm -f *.vtu
	COMMAND rm -f *.pvtu
	COMMAND rm -f *.vts
	COMMAND rm -f *.pvts
)