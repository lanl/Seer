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


#ifdef INSITU_ON
  #include "inSituWrap.hpp"
#endif

// Global log
std::stringstream debugLog;

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


  #ifdef INSITU_ON
	InWrap::InsituWrap insitu;
	insitu.init(argc, argv, myRank, numRanks);
  #endif


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

	  #ifdef INSITU_ON
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


			/*
			{
				vtkSmartPointer<vtkPoints> points =
					vtkSmartPointer<vtkPoints>::New();
				points->InsertNextPoint(1,1,1);
				points->InsertNextPoint(2,2,2);
				points->InsertNextPoint(3,3,3);

				vtkSmartPointer<vtkPolyData> polydata =
					vtkSmartPointer<vtkPolyData>::New();
				polydata->SetPoints(points);

				vtkSmartPointer<vtkFloatArray> floatArray =
					vtkSmartPointer<vtkFloatArray>::New();
				floatArray->SetNumberOfValues(3);
				floatArray->SetNumberOfComponents(1);
				floatArray->SetName("FloatArray");
				for(vtkIdType i = 0; i < 3; i++)
				{
					floatArray->SetValue(i,2);
				}
				polydata->GetPointData()->AddArray(floatArray);


				vtkSmartPointer<vtkIntArray> intArray =
					vtkSmartPointer<vtkIntArray>::New();
				intArray->SetNumberOfValues(3);
				intArray->SetNumberOfComponents(1);
				intArray->SetName("IntArray");
				for(vtkIdType i = 0; i < 3; i++)
				{
					intArray->SetValue(i,2);
				}

				polydata->GetPointData()->AddArray(intArray);

				for(vtkIdType i = 0; i < 3; i++)
				{
					double p[3];
					polydata->GetPoint(i,p);
					vtkFloatArray* pointsFloatArray = vtkFloatArray::SafeDownCast(polydata->GetPointData()->GetArray("FloatArray"));
					vtkIntArray* pointsIntArray = vtkIntArray::SafeDownCast(polydata->GetPointData()->GetArray("IntArray"));
					std::cout << "Point " << i << " : " << p[0] << " " << p[1] << " " << p[2] << " "
							<< pointsFloatArray->GetValue(i) << " " << pointsIntArray->GetValue(i) << std::endl;
				}

				polydata->GetPointData()->NullPoint(1);
				polydata->Modified();

				for(vtkIdType i = 0; i < 3; i++)
				{
					double p[3];
					polydata->GetPoint(i,p);
					vtkFloatArray* pointsFloatArray = vtkFloatArray::SafeDownCast(polydata->GetPointData()->GetArray("FloatArray"));
					vtkIntArray* pointsIntArray = vtkIntArray::SafeDownCast(polydata->GetPointData()->GetArray("IntArray"));
					std::cout << "Point " << i << " : " << p[0] << " " << p[1] << " " << p[2] << " "
							<< pointsFloatArray->GetValue(i) << " " << pointsIntArray->GetValue(i) << std::endl;
				}
			}
			*/

			/*
			temp.strucGrid->Modified();
			
			vtkIdType numberOfCellArrays = temp.strucGrid->GetCellData()->GetNumberOfArrays();
  			std::cout << "Number of CellData arrays: " << numberOfCellArrays << std::endl;
			for(vtkIdType i = 0; i < numberOfCellArrays; i++)
			{
				int dataTypeID = temp.strucGrid->GetCellData()->GetArray(i)->GetDataType();
				std::cout << "Array " << i << ": " << temp.strucGrid->GetCellData()->GetArrayName(i)
						<< " (type: " << dataTypeID << ")" << std::endl;
			}

			std::string arrayName = "vert-data";

			
			vtkIdType idNum = temp.strucGrid->GetNumberOfPoints();
			std::cout << "idNum: " << idNum << std::endl;

			vtkSmartPointer<vtkDoubleArray> array = vtkDoubleArray::SafeDownCast(temp.strucGrid->GetPointData()->GetArray(arrayName.c_str()));
			if (array)
			{
				for(int i = 0; i < idNum; i++)
				{
					double value;
					value = array->GetValue(i);
					std::cout << i << ": " << value << std::endl;
				}
			}
			else
			{
				std::cout << "The file does not have a cellData array named " << arrayName << std::endl;
			}
			*/

			insitu.simData = &temp;


		  #ifdef  CATALYST_ENABLED
			if (insitu.isCatalystOn())
			{
				insitu.cat.coProcess(temp.getGrid(), t / 1.0, t, t == (numTimesteps - 1));
			}
		  #endif //CATALYST_ENABLED

			insitu.timestepExecute(t);

			//insitu.recordEvent( "comp-time_ts_" + std::to_string(t) + "_" + std::to_string(myRank), std::to_string( clock.getDuration("factComputation") ) );
			//insitu.recordEvent( "loop-time_ts_" + std::to_string(t) + "_" + std::to_string(myRank), std::to_string( clock.getDuration("mainLoop") ) );

	
			//temp.writeParts(numRanks, myRank, myRank, "testStructuredMPI_" + std::to_string(t));

			InWrap::writeLog( "myLog_" + std::to_string(myRank), msgLog.str());
		}
	  #endif

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

