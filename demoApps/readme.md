There are 2 demo files:
- miniApp.cpp
- papiDemo.cpp




**miniApp.cpp** is a demo for generating parallel vtk apps. It can be run as a standalone to generate vtk files or with catalyst. It can also be used to Tau for profiling test purposes.

a. Build/Run for generating vtk files  
cmake ..  
make -j16  

cd demoApps/  
mpirun -np 4 miniApp  



b. Build/Run for generating vtk files with TAU enabled
source ~/runTau.sh  
cmake .. -DUSE_TAU=ON  
make -j16  

cd demoApps/  
export TAU_METRICS=PAPI_L1_DCM:PAPI_L1_ICM	#to specify hardware metrics we want to see  
mpirun -np 4 miniApp  
paraprof  #to see Tau Measurement  

run pprof inside MULTI__PAPI_L1_DCM to get a comman line view of things  



c. Build/Run with Catalyst
cmake .. -DENABLE_CATALYST=ON -DParaView_DIR=/home/pascal/software/ParaView-v5.3.0-build/install/lib/cmake/paraview-5.3
make -j16  

cd demoApps/  
export LD_LIBRARY_PATH=/home/pascal/software/ParaView-v5.3.0-build/install/lib/paraview-5.3:$LD_LIBRARY_PATH  

example 1 - offline:  
	mpirun -np 4 miniApp ../../inSitu/test_pointGaussian.py  

example 2 - live:  
	- Start ParaView5.3  
	- Catalyst -> Connect and do not change port  
	mpirun -np 4 miniApp ../../inSitu/test_pointGaussian_live.py  
	- Once the sim starts, click on Extract:input to visualize it and under Catalyst -> Pause/Continue to pause and resume the simulation  





**papiDemo.cpp** is a driver for show how to use the papi hardware counter library. It's a standalone app.

a. Build/Run  
cmake ..  
make -j16  
./papiTest  





**runTau.sh for Linux Desktop:**
#TAU  
export PATH=/home/pascal/software/tau-2.27/install/x86_64/bin:$PATH  
export LD_LIBRARY_PATH=/home/pascal/software/tau-2.27/install/x86_64/lib:$LD_LIBRARY_PATH  
export export TAU_MAKEFILE=/home/pascal/software/tau-2.27/install/x86_64/lib/Makefile.tau-papi-mpi-pdt  
export LD_LIBRARY_PATH=/home/pascal/software/pdtoolkit-3.25/install/x86_64/lib:$LD_LIBRARY_PATH  