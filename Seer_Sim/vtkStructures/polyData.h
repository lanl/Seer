#pragma once

#include <string>
#include <iostream>

#include <mpi.h>


#include <vtkSmartPointer.h>
#include <vtkDoubleArray.h>
#include <vtkPolyData.h>
#include <vtkXMLPPolyDataWriter.h>
#include <vtkSOADataArrayTemplate.h>
#include <vtkAOSDataArrayTemplate.h>
#include <vtkCellArray.h>
#include <vtkPoints.h>
#include <vtkPointData.h>
#include <vtkCellData.h>
#include <vtkDebugLeaksManager.h>
#include <vtkMultiBlockDataSet.h>
#include <vtkNew.h>
#include <vtkTrivialProducer.h>
#include <vtkDataObject.h>


namespace Seer
{

class PolyData: public VTKDataStruct
{
	vtkSmartPointer<vtkXMLPPolyDataWriter> writer;
	vtkSmartPointer<vtkPolyData> polyData;

	vtkSmartPointer<vtkPoints> pnts;

	
  public:
	PolyData();
	~PolyData() {};

	vtkSmartPointer<vtkDataSet> getGrid(){ return polyData; }
	vtkSmartPointer<vtkPolyData> getPolyData() { return polyData; }

	// Topology
	template <typename T> void addPoint(T *pointData, int _dims = 3);
	void setPoints(vtkSmartPointer<vtkPoints> _pnts) { polyData->SetPoints(_pnts); }
	void pushPointsToGrid() { polyData->SetPoints(pnts); }


	// Data
	template <typename T> void addScalarPointData(std::string varName, int numPoints, T *data);
	template <typename T> void addVectorPointData(std::string varName, int numPoints, int numComponents, T *data);
	template <typename T> void addScalarCellData(std::string varName, int numPoints, T *data);
	template <typename T> void addVectorCellData(std::string varName, int numPoints, int numComponents, T *data);
	template <typename T> void addFieldData(std::string fieldName, T *data);


	// Writing
	void writeParts(int numPieces, int startPiece, int SetEndPiece, std::string fileName);
	void write(std::string fileName, int parallel = 0);


	// Set n Get
	int getNumVertices() { return polyData->GetNumberOfVerts(); }
};



inline PolyData::PolyData()
{
	writer = vtkSmartPointer<vtkXMLPPolyDataWriter>::New();
	polyData = vtkSmartPointer<vtkPolyData>::New();

	pnts = vtkSmartPointer<vtkPoints>::New();
}



template <typename T>
inline void PolyData::addPoint(T *pointData, int _dims)
{
	if (_dims == 1)
		pnts->InsertNextPoint(pointData[0], 0, 0);
	else if (_dims == 2)
		pnts->InsertNextPoint(pointData[0], pointData[1], 0);
	else
		pnts->InsertNextPoint(pointData[0], pointData[1], pointData[2]);
}


// Attributes
template <typename T>
inline void PolyData::addFieldData(std::string fieldName, T *data)
{
  	vtkAOSDataArrayTemplate<T>* temp = vtkAOSDataArrayTemplate<T>::New();

  	temp->SetNumberOfTuples(1);
  	temp->SetNumberOfComponents(1);
  	temp->SetName(fieldName.c_str());
  	temp->SetArray(data, 1, false, true);

  	polyData->GetFieldData()->AddArray(temp);
}

//
// Data
template <typename T>
inline void PolyData::addScalarPointData(std::string varName, int numPoints, T *data)
{
	vtkSOADataArrayTemplate<T>* temp = vtkSOADataArrayTemplate<T>::New();

	temp->SetNumberOfTuples(numPoints);
	temp->SetNumberOfComponents(1);
	temp->SetName(varName.c_str());
	temp->SetArray(0, data, numPoints, false, true);
	polyData->GetPointData()->AddArray(temp);

	temp->Delete();
}


template <typename T>
inline void PolyData::addVectorPointData(std::string varName, int numPoints, int numComponents, T *data)
{
	vtkAOSDataArrayTemplate<T>* temp = vtkAOSDataArrayTemplate<T>::New();

	temp->SetNumberOfTuples(numPoints);
	temp->SetNumberOfComponents(numComponents);
	temp->SetName(varName.c_str());
	temp->SetArray(data, numPoints * numComponents, false, true);
	polyData->GetPointData()->AddArray(temp);

	temp->Delete();
}


template <typename T>
inline void PolyData::addScalarCellData(std::string varName, int numPoints, T *data)
{
	vtkSOADataArrayTemplate<T>* temp = vtkSOADataArrayTemplate<T>::New();

	temp->SetNumberOfComponents(1);
	temp->SetNumberOfTuples(numPoints);
	temp->SetName(varName.c_str());
	temp->SetArray(0, data, numPoints, false, true);
	polyData->GetCellData()->AddArray(temp);

	temp->Delete();
}


template <typename T>
inline void PolyData::addVectorCellData(std::string varName, int numPoints, int numComponents, T *data)
{
	vtkAOSDataArrayTemplate<T>* temp = vtkAOSDataArrayTemplate<T>::New();

	temp->SetNumberOfComponents(numComponents);
	temp->SetNumberOfTuples(numPoints);
	temp->SetName(varName.c_str());
	temp->SetArray(data, numPoints * numComponents, false, true);
	polyData->GetCellData()->AddArray(temp);

	temp->Delete();
}



//
// Writing
inline void PolyData::writeParts(int numPieces, int startPiece, int endPiece, std::string fileName)
{
	writer->SetNumberOfPieces(numPieces);
	writer->SetStartPiece(startPiece);
	writer->SetEndPiece(endPiece);

	write(fileName, 1);
}

inline void PolyData::write(std::string fileName, int parallel)
{
	std::string outputFilename;
	if (parallel == 1)
		outputFilename = fileName + ".pvtp";
	else
		outputFilename = fileName + ".vtp";

	writer->SetDataModeToBinary();
    writer->SetCompressor(nullptr);
	writer->SetFileName(outputFilename.c_str());

	#if VTK_MAJOR_VERSION <= 5
	writer->SetInput(polyData);
	#else
	writer->SetInputData(polyData);
	#endif

	writer->Write();
}

} // inwrap

