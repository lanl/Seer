#include <iostream>
#include <mpi.h>
#include <chrono>
#include <thread>
#include <vector>
#include <chrono>
#include <thread>


#ifdef INSITU_ON
	#include "inSituWrap.hpp"

	#ifdef CATALYST_ENABLED
		#include "catalystAdaptor.h"
	#elif SENSEI_ENABLED
		#include "senseiBridge.h"
	#endif

#endif


//#ifdef CATALYST_ENABLED
//	#include "catalystAdaptor.h"
//#endif


int main(int argc, char *argv[])
{
	int myRank, numRanks, threadSupport;
	MPI_Init_thread(NULL, NULL, MPI_THREAD_MULTIPLE, &threadSupport);
	MPI_Comm_size(MPI_COMM_WORLD, &numRanks);
	MPI_Comm_rank(MPI_COMM_WORLD, &myRank);

	int numPoints = 100;
	int numTimesteps = 300;


  #ifdef INSITU_ON
	InWrap::InsituWrap insitu;
	insitu.init(argc, argv, myRank, numRanks);
  #endif


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


  #ifdef SENSEI_ENABLED

	bool SenseiOn = false;

	if (argc > 1)
		SenseiOn = InWrap::checkSensei(argv[1]);

	if (SenseiOn)
	{
		std::cout << "Sensei on" << std::endl;
		InWrap::senseiInitialize(MPI_COMM_WORLD, argv[2]);
	}

  #endif


	MPI_Barrier(MPI_COMM_WORLD);

	for (int t = 0; t < numTimesteps; t++)
	{
	  #ifdef INSITU_ON
		insitu.timestepInit();
	  #endif

		double *data = new double[numPoints];
		std::vector<double> points;

		for (int i = 0; i < numPoints ; i++)
		{
			double pnt[3];
			pnt[0] = i;	pnt[1] = myRank + (t / 10.0);	pnt[2] = myRank;
			
			points.push_back(pnt[0]);
			points.push_back(pnt[1]);
			points.push_back(pnt[2]);

			data[i] = myRank;
		}


      #ifdef INSITU_ON
		InWrap::UnstructuredGrid temp;
		temp.setPoints(&points[0], numPoints, VTK_VERTEX);
		temp.addScalarData("pressure", numPoints, data);
		
		temp.addFieldData("time", &t);
		temp.addFieldData("rank", &myRank);
		temp.addFieldData("numRank", &numRanks);


	  #ifdef  CATALYST_ENABLED
		if (insitu.isCatalystOn())
		{
			insitu.cat.coProcess(temp.getGrid(), t / 1.0, t, t == (numTimesteps - 1));
			//std::this_thread::sleep_for(std::chrono::milliseconds(3000));		// uncomment  for Catalyst LIVE
		}
		//else
		//	temp.writeParts(numRanks, myRank, myRank, "miniAppUns_" + std::to_string(t));
	 
	  #elif SENSEI_ENABLED

		if (SenseiOn)
		{
			InWrap::senseiAnalyze("mesh", temp.getGrid(), t, t/1.0);
		}
		else
			temp.writeParts(numRanks, myRank, myRank, "miniAppUns_" + std::to_string(t));

	  #else
		//temp.writeParts(numRanks, myRank, myRank, "miniAppUns_" + std::to_string(t));
      #endif 

		insitu.timestepExecute(t);

	  #endif //insitu


		if (myRank == 0)
			std::cout << myRank << " ~ ts: " << t << std::endl;

		std::this_thread::sleep_for(std::chrono::milliseconds(5000));
		MPI_Barrier(MPI_COMM_WORLD);
	}


  #ifdef SENSEI_ENABLED
	if (SenseiOn)
		InWrap::senseiFinalize();
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
