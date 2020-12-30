# Catalyst
if (ENABLE_CATALYST)
	message("CATALYST_ENABLED demo")
	add_executable(miniAppUnstruc miniAppUnstruc.cpp)
	add_executable(miniAppStructured miniAppStructured.cpp)

	target_link_libraries(miniAppStructured ${MPI_LIBRARIES} ${SEER_LIBS} papi vtkPVPythonCatalyst-pv5.7 vtkParallelMPI-pv5.7 margo sdskv-client)
	target_link_libraries(miniAppUnstruc ${MPI_LIBRARIES} ${SEER_LIBS} papi vtkPVPythonCatalyst-pv5.7 vtkParallelMPI-pv5.7 margo sdskv-client)
	

	#include(vtkModuleMacros)
  	#include(vtkMPI)
  
elseif (SENSEI_ENABLED)
	message("SENSEI_ENABLED")

	target_link_libraries(miniAppStructured ${MPI_LIBRARIES} ${SEER_LIBS} papi margo sdskv-client sensei senseiCore)
	target_link_libraries(miniAppUnstruc ${MPI_LIBRARIES} ${SEER_LIBS} papi margo sdskv-client sensei senseiCore)

	#include(vtkModuleMacros)
  	#include(vtkMPI)

else()
	message("VTK_ENABLED demoapps")
	add_executable(miniAppUnstruc miniAppUnstruc.cpp)
	add_executable(miniAppStructured miniAppStructured.cpp)

	target_link_libraries(miniAppStructured ${MPI_LIBRARIES} ${SEER_LIBS} ${VTK_LIBRARIES} papi margo sdskv-client)
	target_link_libraries(miniAppUnstruc ${MPI_LIBRARIES} ${SEER_LIBS} ${VTK_LIBRARIES} papi margo sdskv-client)
endif()



# Remove output files
add_custom_target(clean-demo
	COMMAND rm -f *.vtu
	COMMAND rm -f *.pvtu
	COMMAND rm -f *.vts
	COMMAND rm -f *.pvts
)