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

#include "timer.hpp"
#include "seerInSituWrap.hpp"



int fib(int n) 
{ 
   if (n <= 1) 
      return n; 
   return fib(n-1) + fib(n-2); 
} 


int main(int argc, char *argv[])
{
	std::stringstream msgLog;

	int myRank, numRanks, threadSupport;
	MPI_Init_thread(NULL, NULL, MPI_THREAD_MULTIPLE, &threadSupport);
	MPI_Comm_size(MPI_COMM_WORLD, &numRanks);
	MPI_Comm_rank(MPI_COMM_WORLD, &myRank);

	int numPoints = 100;
	int numTimesteps = 300;

	srand(time(NULL) + myRank);



	Seer::SeerInsituWrap insitu;
	insitu.init(argc, argv, myRank, numRanks);



	char processor_name[256];
	int processor_name_len;
	MPI_Get_processor_name(processor_name, &processor_name_len);

	InWrap::Timer clock;

	if (myRank == 0)
		std::cout << numTimesteps << std::endl;

	MPI_Barrier(MPI_COMM_WORLD);


	// Loop over timesteps
	for (int t = 0; t < numTimesteps; t++)
	{
		clock.start("mainLoop");

		if (myRank == 0)
			std::cout  << "ts: " << t << std::endl;

	  #ifdef INSITU_ON
		insitu.timestepInit();
	  #endif


		msgLog << "\nTimestep: " << t << std::endl;

		int dimX, dimY, dimZ, dims[3];
		dims[0] = dimX = 2; 
		dims[1] = dimY = 6; 
		dims[2] = dimZ = 8;


		std::vector<double> points;
		int numPoints = 0;
		int numCells = 35;

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

					msgLog << "Point: " << pnt[0] << ", " << pnt[1] << ", " << pnt[2] << std::endl;
				}



		// vertex centered -  scalar
		double *pointData = new double[numPoints];
		for (int i = 0; i < numPoints ; i++)
			pointData[i] = myRank + t * 0.01;


		// cell centered - scalar
		double *cellData = new double[numCells];
		double *cellDataFact = new double[numCells];
		int *cellRankData = new int[numCells];

		clock.start("factComputation");

		for (int i = 0; i < numCells ; i++)
		{

			// Do some computatiton
			int number = rand()%40 + 1;
			int f = fib( number);
			cellDataFact[i] = f/1000.0;
		

			cellData[i] = myRank + t * 0.05;
			cellRankData[i] = myRank;
		}

		clock.stop("factComputation");

		msgLog << "\nnumPoints: " << numPoints << " point data: " << myRank + t * 0.01 << std::endl;
		msgLog << "numCells: " << numCells << " cell data: " << myRank + t * 0.05 << std::endl;

		clock.stop("mainLoop");


		MPI_Barrier(MPI_COMM_WORLD);

		if (insitu.isInsituOn())
		{
			float tempVector[3];
			tempVector[0] = myRank;
			tempVector[1] = myRank+0.2;
			tempVector[2] = myRank+0.4;


			InWrap::StructuredGrid temp;


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

		  #ifdef  CATALYST_ENABLED
			if (insitu.isCatalystOn())
			{
				insitu.cat.coProcess(temp.getGrid(), t / 1.0, t, t == (numTimesteps - 1));
			}
		  #endif //CATALYST_ENABLED

			insitu.timestepExecute(t);
	
			//temp.writeParts(numRanks, myRank, myRank, "testStructuredMPI_" + std::to_string(t));

			InWrap::writeLog( "myLog_" + std::to_string(myRank), msgLog.str());
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(5000));
		MPI_Barrier(MPI_COMM_WORLD);
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

