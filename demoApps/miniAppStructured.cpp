#include <iostream>
#include <sstream>
#include <vector>
#include <chrono>
#include <thread>
#include <cassert>
#include <chrono>
#include <thread>

#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <mpi.h>
#include <math.h>

#include "timer.hpp"
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


double distance(float x1, float y1, float z1, float x2, float y2, float z2)
{
	return sqrt( (x1-x2)*(x1-x2) + (y1-y2)*(y1-y2) + (z1-z2)*(z1-z2) );
}

int main(int argc, char *argv[])
{
	//std::stringstream msgLog;

	int myRank, numRanks, threadSupport;
	char processor_name[256];
	int processor_name_len;
	MPI_Init_thread(NULL, NULL, MPI_THREAD_MULTIPLE, &threadSupport);
	MPI_Comm_size(MPI_COMM_WORLD, &numRanks);
	MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
	MPI_Get_processor_name(processor_name, &processor_name_len);

	int numTimesteps = 500;

	srand(time(NULL) + myRank);


	Seer::SeerInsituWrap insitu;
	insitu.init(argc, argv, myRank, numRanks);


	Seer::Timer clock;

	if (myRank == 0)
		std::cout << numTimesteps << std::endl;


	//
	// Sim setup
	int dimX, dimY, dimZ, dims[3];
	dims[0] = dimX = 5; 
	dims[1] = dimY = 7; 
	dims[2] = dimZ = 9;

	int numPoints = 0;
	int numCells = (dimX-1) * (dimY-1) * (dimZ-1);

	std::vector<double> points;
	for (int _z = 0; _z < dimZ; _z++)
		for (int _y = 0; _y < dimY; _y++)
			for (int _x = 0; _x < dimX; _x++)
			{
				double pnt[3];
				pnt[0] = _x + myRank;	pnt[1] = _y;	pnt[2] = _z;

				points.push_back(pnt[0]);
				points.push_back(pnt[1]);
				points.push_back(pnt[2]);

				numPoints++;

				//msgLog << "Point: " << pnt[0] << ", " << pnt[1] << ", " << pnt[2] << std::endl;
			}

	// explosion
	float explosionX = 2.5;
	float explosionY = 3;
	float explosionZ = 4;
	float explosionValue = 500;

	MPI_Barrier(MPI_COMM_WORLD);


	// Loop over timesteps
	for (int t = 0; t < numTimesteps; t++)
	{
	  clock.start("mainLoop");

		insitu.timestepInit();

		Seer::log << "\nTimestep: " << t << std::endl;

		// vertex centered -  scalar
		double *pointData = new double[numPoints];
		for (int i = 0; i < numPoints ; i++)
			pointData[i] = myRank + t * 0.01;

		// cell centered - scalar
		double *cellData = new double[numCells];
		double *cellDataFact = new double[numCells];
		int *cellRankData = new int[numCells];
		double *explosionValues = new double[numCells];

	  clock.start("factComputation");

		for (int i = 0; i < numCells ; i++)
		{
			// Do some computatiton
			int number = rand()%40 + 1;
			int f = fib( number*myRank );
			cellDataFact[i] = f/1000.0;
		
			cellData[i] = myRank + t * 0.05;
			cellRankData[i] = myRank;
		}

		int index = 0;
		for (int _z = 0; _z < dimZ-1; _z++)
			for (int _y = 0; _y < dimY-1; _y++)
				for (int _x = 0; _x < dimX-1; _x++)
				{
					explosionValues[index] = explosionValue/distance(_x,_y,_z, explosionX, explosionY, explosionZ);
				}
		explosionValue--;

	  clock.stop("factComputation");
	  clock.stop("mainLoop");

		Seer::log << "\nnumPoints: " << numPoints << " point data: " << myRank + t * 0.01 << std::endl;
		Seer::log << "numCells: " << numCells << " cell data: " << myRank + t * 0.05 << std::endl;

	  
		if (insitu.isInsituOn())
		{
			MPI_Barrier(MPI_COMM_WORLD);
			if (myRank == 0)
				std::cout << "Enter mainLoop..." << std::endl;
				
			insitu.simEvents["mainLoop"] = std::to_string(clock.getDuration("mainLoop"));
			insitu.simEvents["factComputation"] = std::to_string(clock.getDuration("factComputation"));

			MPI_Barrier(MPI_COMM_WORLD);
			if (myRank == 0)
				std::cout << "Enter mainLoop done!" << std::endl;

			float tempVector[3];
			tempVector[0] = myRank;
			tempVector[1] = myRank+0.2;
			tempVector[2] = myRank+0.4;

		  #ifdef  CATALYST_ENABLED
			Seer::StructuredGrid temp;
			{
				temp.setWholeExtents(0,numRanks, 0,dimY-1, 0,dimZ-1);

				temp.setDims(dimX, dimY, dimZ);
				temp.setExtents(myRank,myRank+1, 0,dimY-1, 0,dimZ-1);


				temp.setPoints(&points[0], numPoints);
				temp.addFieldData("processor_name", tempVector, 3);
				
				temp.addFieldData("myRank", &myRank);
				temp.addFieldData("numRanks", &numRanks);
				temp.addFieldData("tempp_" + std::to_string(myRank), &myRank);

				temp.addScalarPointData("vert-data", numPoints, pointData);
				temp.addScalarCellData("cell-data-scalar", numCells, cellData);
				temp.addScalarCellData("cell-data-fact", numCells, cellDataFact);
				temp.addScalarCellData("_rank", numCells, cellRankData);
			}

		  
			if (insitu.isCatalystOn())
			{
				insitu.cat.coProcess(temp.getGrid(), t / 1.0, t, t == (numTimesteps - 1));
			}
		  #endif //CATALYST_ENABLED

			insitu.timestepExecute(t);
	
			//temp.writeParts(numRanks, myRank, myRank, "testStructuredMPI_" + std::to_string(t));

			Seer::writeLog( "logs/myLog_" + std::to_string(myRank), Seer::log.str());
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(5000));
		MPI_Barrier(MPI_COMM_WORLD);

		if (myRank == 0)
			std::cout << "ts: " << t << std::endl;
	}

	MPI_Finalize();
	return 0;
}

//
// Running:

// vtk output:
// mpirun -np 4 demoApps/miniAppStructured --insitu ../inSitu/inputs/inputs.json

// With Catalyst:
// mpirun -np 4 demoApps/miniAppStructured --catalyst ../inSitu/scripts/PV5.5.1/miniAppStructured_views.py
// mpirun -np 4 demoApps/miniAppStructured --catalyst ../inSitu/scripts/write_vtm.py

// mpirun -np 4 demoApps/miniAppStructured --sensei ../inSitu/scripts/hist_py.xml

