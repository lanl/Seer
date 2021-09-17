#include <iostream>
#include <mpi.h>
#include <chrono>
#include <thread>
#include <vector>
#include <chrono>
#include <thread>
#include "utils.hpp"


#include "seerInSituWrap.hpp"


#ifdef CATALYST_ENABLED
	#include "catalystAdaptor.h"
#endif


std::stringstream Seer::log;
std::string Seer::logName = "";


int fib(int n) 
{ 
   if (n <= 1) 
      return n; 
   return fib(n-1) + fib(n-2); 
} 


int main(int argc, char *argv[])
{
	int myRank, numRanks, threadSupport;
	MPI_Init_thread(NULL, NULL, MPI_THREAD_MULTIPLE, &threadSupport);
	MPI_Comm_size(MPI_COMM_WORLD, &numRanks);
	MPI_Comm_rank(MPI_COMM_WORLD, &myRank);

	int numPoints = 100;
	int numTimesteps = 300;
	int numVars = 2;

	std::vector<std::string> vars;
	vars.push_back("temperature");
	vars.push_back("pressure");

	float *pressureData = new float[numPoints];
	float *temperatureData = new float[numPoints]; 

	float *_x = new float[numPoints]; 
	float *_y = new float[numPoints]; 
	float *_z = new float[numPoints]; 

	std::cout << "numPoints: " << numPoints << std::endl;

	



	Seer::SeerInsituWrap insitu;
	insitu.init(argc, argv, myRank, numRanks);


	if (myRank == 0)
		//insitu.simEvents["variables"] = "temperature,pressure";
		insitu.recordEvent("variables","temperature,pressure");


	int window = 10;
	int gid = 1;

	int domains[3];
	domains[0] = 64;
	domains[1] = 64;
	domains[2] = 64;
	std::string config_file;

	int extents[6];
	extents[0] = 0; extents[1] = domains[0]; 
	extents[2] = 0; extents[3] = domains[1];
	extents[3] = 0; extents[5] = domains[2];


	std::vector<int> indices = Seer::shuffleArray(numPoints);
	if (myRank == 0)
		for (int i=0; i<numPoints; i++)
			std::cout << indices[i] << ", " << std::endl;


	Seer::Timer clock;

	if (myRank == 0)
		std::cout << "Stating loop..." << std::endl;

	MPI_Barrier(MPI_COMM_WORLD);

	for (int t = 0; t < numTimesteps; t++)
	{
		MPI_Barrier(MPI_COMM_WORLD);
		if (myRank == 0)
			std::cout << "ts: " << t << std::endl;

		insitu.timestepInit();

		MPI_Barrier(MPI_COMM_WORLD);
		if (myRank == 0)
			std::cout << " after timestepInit" << std::endl;

		clock.start("mainLoop");

		float *data = new float[numPoints];
		std::vector<float> points;

		// Determine point position
		for (int i = 0; i < numPoints; i++)
		{
			float pnt[3];
			pnt[0] = i;	pnt[1] = myRank + (t / 10.0);	pnt[2] = myRank;

			_x[i] = pnt[0];
			_y[i] = pnt[1];
			_z[i] = pnt[2];
			
			points.push_back(pnt[0]);
			points.push_back(pnt[1]);
			points.push_back(pnt[2]);

			data[i] = myRank;
		}

		MPI_Barrier(MPI_COMM_WORLD);
		if (myRank == 0)
			std::cout << " factComp ..." << std::endl;

		clock.start("factComp");

		// Create some dummy values
		//std::cout << myRank << " ~ " << "factComputation - dummy values " << std::endl;
		for (int i = 0; i < numPoints; i++)
		{
			// Do some computatiton
			int number = rand()%5 + 1;
			int f = fib( number*myRank );

			pressureData[i] = f/1000.0;
			temperatureData[i] = myRank + t * 0.05;
		}

		clock.stop("factComp");
		clock.stop("mainLoop");


		MPI_Barrier(MPI_COMM_WORLD);
		if (myRank == 0)
			std::cout << " store results ..." << std::endl;


		std::string tempKey, tempValue;

		tempKey = "mainLoop_ts_" + std::to_string(t) + "_rank_" +  std::to_string(myRank);
		tempValue = std::to_string(clock.getDuration("mainLoop"));
		//insitu.simEvents[tempKey] = tempValue;
		insitu.recordEvent(tempKey, tempValue);

		tempKey = "factComp_ts_" + std::to_string(t) + "_rank_" +  std::to_string(myRank);
		tempValue = std::to_string(clock.getDuration("factComp"));
		insitu.recordEvent(tempKey, tempValue);
		//insitu.simEvents[tempKey] = tempValue;




		tempKey = "pressure_ts_" + std::to_string(t) + "_rank_" +  std::to_string(myRank);
		tempValue = Seer::serializeArray(pressureData, numPoints, 0.5, indices);
		//std::cout << "pressureData- tempValue:" << pressureData << tempValue << std::endl;
		insitu.recordEvent(tempKey, tempValue);

		tempKey = "temperature_ts_" + std::to_string(t) + "_rank_" +  std::to_string(myRank);
		tempValue = Seer::serializeArray(temperatureData, numPoints, 0.5, indices);
		insitu.recordEvent(tempKey, tempValue);

		tempKey = "x_ts_" + std::to_string(t) + "_rank_" +  std::to_string(myRank);
		tempValue = Seer::serializeArray(_x, numPoints, 0.5, indices);
		insitu.recordEvent(tempKey, tempValue);

		tempKey = "y_ts_" + std::to_string(t) + "_rank_" +  std::to_string(myRank);
		tempValue = Seer::serializeArray(_y, numPoints, 0.5, indices);
		insitu.recordEvent(tempKey, tempValue);

		tempKey = "z_ts_" + std::to_string(t) + "_rank_" +  std::to_string(myRank);
		tempValue = Seer::serializeArray(_z, numPoints, 0.5, indices);
		insitu.recordEvent(tempKey, tempValue);
		//insitu.simEvents["mainLoop"] = std::to_string(clock.getDuration("mainLoop"));


	  #ifdef  CATALYST_ENABLED
		Seer::UnstructuredGrid temp;
		temp.setPoints(&points[0], numPoints, VTK_VERTEX);
		temp.addScalarData("pressure", numPoints, data);
		
		temp.addFieldData("time", &t);
		temp.addFieldData("rank", &myRank);
		temp.addFieldData("numRank", &numRanks);


	  
		if (insitu.isCatalystOn())
		{
			insitu.cat.coProcess(temp.getGrid(), t / 1.0, t, t == (numTimesteps - 1));
		}
		//else
		//	temp.writeParts(numRanks, myRank, myRank, "miniAppUns_" + std::to_string(t));
	
      #endif 


		MPI_Barrier(MPI_COMM_WORLD);
		if (myRank == 0)
			std::cout << " insitu.timestepExecute ..." << std::endl;

		insitu.timestepExecute(t);

		MPI_Barrier(MPI_COMM_WORLD);
		if (myRank == 0)
			std::cout << " insitu.timestepExecute!" << std::endl;


		Seer::writeLog( "logs/myLog_" + std::to_string(myRank), Seer::log.str());

		if (myRank == 0)
			std::cout << myRank << " ~ ts: " << t << std::endl;

		std::this_thread::sleep_for(std::chrono::milliseconds(5000));
		MPI_Barrier(MPI_COMM_WORLD);
	}


  #ifdef SENSEI_ENABLED
	if (SenseiOn)
		Seer::senseiFinalize();
  #endif

	MPI_Finalize();
}

//
// Running:

// vtk output:
// mpirun -np 4 demoApps/miniAppUnstruc

// With Catalyst:

// mpirun -np 4 demoApps/miniAppUnstruc --catalyst ../inSitu/scripts/write_vtm.py
// mpirun -np 4 demoApps/miniAppUnstruc --catalyst ../inSitu/scripts/PV5.5.1/miniAppUnstr_views.py

// mpirun -np 4 demoApps/miniAppUnstruc --sensei ../inSitu/scripts/hist_py.xml

// Standard way:
// mpirun -np 8 demoApps/miniAppUnstruc --insitu ../inputs/input-test.json