#pragma once

#include <string>
#include <iostream>
#include <sstream>


#include "../utils/timer.hpp"
#include "vtkDataStruct.h"

#include <vtkSmartPointer.h>
#include <vtkDataSet.h>
#include <vtkDoubleArray.h>
#include <vtkRectilinearGrid.h>
#include <vtkXMLPRectilinearGridWriter.h>
#include <vtkSOADataArrayTemplate.h>
#include <vtkAOSDataArrayTemplate.h>
#include <vtkPoints.h>
#include <vtkNew.h>
#include <vtkTrivialProducer.h>



namespace InWrap
{

class RectilinearGrid: public VTKDataStruct
{
	vtkSmartPointer<vtkXMLPRectilinearGridWriter> writer;
	vtkSmartPointer<vtkRectilinearGrid> rectGrid;

	vtkSmartPointer<vtkPoints> pnts;

	vtkSmartPointer<vtkDoubleArray> xCoords, yCoords, zCoords;

	int dims[3];
	int extents[6];
	int wholeExtents[6];

	std::stringstream log;

public:
	RectilinearGrid();
	~RectilinearGrid(){};

	vtkSmartPointer<vtkRectilinearGrid> getRGrid(){ return rectGrid; }
	vtkSmartPointer<vtkDataSet> getGrid(){ return rectGrid; }

	// Topology
	//void pushPointsToGrid(){ rectGrid->SetPoints(pnts); }
	template <typename T>  void addPoint(T *pointData, int _dims);
	void setDims(int x, int y, int z);
	void setExtents(int minX, int maxX,  int minY, int maxY,  int minZ, int maxZ);
	void setWholeExtents(int minX, int maxX,  int minY, int maxY,  int minZ, int maxZ);
	void setXCoordinates(vtkSmartPointer<vtkDataArray> xCoords){ rectGrid->SetXCoordinates(xCoords); }
	void setYCoordinates(vtkSmartPointer<vtkDataArray> yCoords){ rectGrid->SetYCoordinates(yCoords); }
	void setZCoordinates(vtkSmartPointer<vtkDataArray> zCoords){ rectGrid->SetZCoordinates(zCoords); }

	// Data
	template <typename T> void addScalarPointData(std::string varName, int numPoints, T *data);
	template <typename T> void addVectorPointData(std::string varName, int numPoints, int numComponents, T *data);
	template <typename T> void addScalarCellData(std::string varName, int numPoints, T *data);
	template <typename T> void addVectorCellData(std::string varName, int numPoints, int numComponents, T *data);
	template <typename T> void addFieldData(std::string fieldName, T *data);

	// Writing
	void writeParts(int numPieces, int startPiece, int SetEndPiece, std::string fileName);
	void write(std::string fileName, int parallel=1);


	// Set n Get
	int getNumVertices(){ return rectGrid->GetNumberOfPoints(); }
	int getNumCells(){ return rectGrid->GetNumberOfCells(); }

	// log
	std::string getLog(){ return log.str(); }
};



inline RectilinearGrid::RectilinearGrid()
{
	Timer clock(1);

	writer = vtkSmartPointer<vtkXMLPRectilinearGridWriter>::New();
	rectGrid = vtkSmartPointer<vtkRectilinearGrid>::New();

	pnts = vtkSmartPointer<vtkPoints>::New();

	log << "RectilinearGrid RectilinearGrid took: " << clock.stop(1) << " s" << std::endl;
}



inline void RectilinearGrid::setDims(int x, int y, int z)
{ 
	dims[0] = x; 	dims[1] = y; 	dims[2] = z; 
	rectGrid->SetDimensions(dims);
}


inline void RectilinearGrid::setExtents(int minX, int maxX, int minY, int maxY, int minZ, int maxZ)
{ 
	extents[0] = minX; 	extents[1] = maxX;
	extents[2] = minY; 	extents[3] = maxY;
	extents[4] = minZ; 	extents[5] = maxZ; 

	rectGrid->SetExtent(extents); 
}


inline void RectilinearGrid::setWholeExtents(int minX, int maxX, int minY, int maxY, int minZ, int maxZ)
{ 
	wholeExtents[0] = minX; 	wholeExtents[1] = maxX;
	wholeExtents[2] = minY; 	wholeExtents[3] = maxY;
	wholeExtents[4] = minZ; 	wholeExtents[5] = maxZ; 
}


template <typename T> 
inline void RectilinearGrid::addPoint(T *pointData, int _dims)
{	
	Timer clock(1);

	if (_dims == 1)
	{
		//pnts->InsertNextPoint(pointData[0], 0, 0);
	}
	else if (_dims == 2)
		pnts->InsertNextPoint(pointData[0], pointData[1], 0);
	else
		pnts->InsertNextPoint(pointData[0], pointData[1], pointData[2]);

	log << "RectilinearGrid addPoint took: " << clock.stop(1) << " s" << std::endl;
}



//
// Data
template <typename T>
inline void RectilinearGrid::addScalarPointData(std::string varName, int numPoints, T *data)
{
	Timer clock(1);

	vtkSOADataArrayTemplate<T>* temp = vtkSOADataArrayTemplate<T>::New();

	temp->SetNumberOfTuples(numPoints);
	temp->SetNumberOfComponents(1);
	temp->SetName(varName.c_str());
	temp->SetArray(0, data, numPoints, false, true);
	rectGrid->GetPointData()->AddArray(temp);

	temp->Delete();

	log << "RectilinearGrid addScalarPointData took: " << clock.stop(1) << " s" << std::endl;
}


template <typename T>
inline void RectilinearGrid::addVectorPointData(std::string varName, int numPoints, int numComponents, T *data)
{
	Timer clock(1);

	vtkAOSDataArrayTemplate<T>* temp = vtkAOSDataArrayTemplate<T>::New();

	temp->SetNumberOfTuples(numPoints);
	temp->SetNumberOfComponents(numComponents);
	temp->SetName(varName.c_str());
	temp->SetArray(data, numPoints*numComponents, false, true);
	rectGrid->GetPointData()->AddArray(temp);

	temp->Delete();

	log << "RectilinearGrid addVectorPointData took: " << clock.stop(1) << " s" << std::endl;
}


template <typename T>
inline void RectilinearGrid::addScalarCellData(std::string varName, int numPoints, T *data)
{
	Timer clock(1);

	vtkSOADataArrayTemplate<T>* temp = vtkSOADataArrayTemplate<T>::New();

	temp->SetNumberOfComponents(1);
	temp->SetNumberOfTuples(numPoints);
	temp->SetName(varName.c_str());
	temp->SetArray(0, data, numPoints, false, true);
	rectGrid->GetCellData()->AddArray(temp);

	temp->Delete();

	log << "RectilinearGrid addScalarCellData took: " << clock.stop(1) << " s" << std::endl;
}


template <typename T>
inline void RectilinearGrid::addVectorCellData(std::string varName, int numPoints, int numComponents, T *data)
{
	Timer clock(1);

	vtkAOSDataArrayTemplate<T>* temp = vtkAOSDataArrayTemplate<T>::New();

	temp->SetNumberOfComponents(numComponents);
	temp->SetNumberOfTuples(numPoints);
	temp->SetName(varName.c_str());
	temp->SetArray(data, numPoints*numComponents, false, true);
	rectGrid->GetCellData()->AddArray(temp);

	temp->Delete();

	log << "RectilinearGrid addVectorCellData took: " << clock.stop(1) << " s" << std::endl;
}


template <typename T>
inline void RectilinearGrid::addFieldData(std::string fieldName, T *data)
{
	Timer clock(1);

  	vtkAOSDataArrayTemplate<T>* temp = vtkAOSDataArrayTemplate<T>::New();

  	temp->SetNumberOfTuples(1);
  	temp->SetNumberOfComponents(1);
  	temp->SetName(fieldName.c_str());
  	temp->SetArray(data, 1, false, true);

  	rectGrid->GetFieldData()->AddArray(temp);
  	temp->Delete();


	log << "RectilinearGrid addFieldData took: " << clock.stop(1) << " s" << std::endl;	  
}

//
// Writing
inline void RectilinearGrid::writeParts(int numPieces, int startPiece, int endPiece, std::string fileName)
{
	Timer clock(1);

	writer->SetNumberOfPieces(numPieces);
	writer->SetStartPiece(startPiece);
	writer->SetEndPiece(endPiece);

	write(fileName);

	log << "RectilinearGrid writeParts took: " << clock.stop(1) << " s" << std::endl;
}

inline void RectilinearGrid::write(std::string fileName, int parallel)
{	
	Timer clock(1);

	std::string outputFilename;
	if (parallel == 1)
	{
		outputFilename = fileName + ".pvtr";

		vtkNew<vtkTrivialProducer> tp;
   		tp->SetOutput(rectGrid);
   		tp->SetWholeExtent(wholeExtents[0], wholeExtents[1],
   							wholeExtents[2], wholeExtents[3],
   							wholeExtents[4], wholeExtents[5]);

    	writer->SetInputConnection(tp->GetOutputPort());
	}
	else
		outputFilename = fileName + ".vtr";

	writer->SetFileName(outputFilename.c_str());

  #if VTK_MAJOR_VERSION <= 5
	writer->SetInput(rectGrid);
  #else
	writer->SetInputData(rectGrid);
  #endif

	writer->Write();

	log << "RectilinearGrid write took: " << clock.stop(1) << " s" << std::endl;
}

} // inwrap

