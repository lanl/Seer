#pragma once

#include <vtkSmartPointer.h>
#include <vtkNew.h>
#include <vtkCPDataDescription.h>
#include <vtkCPInputDataDescription.h>
#include <vtkCPProcessor.h>
#include <vtkCPPythonScriptPipeline.h>
#include <vtkMPI.h>
#include <vtkDataObject.h>

#include "unstructuredGrid.h"

#include <mpi.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>



class CatalystAdaptor
{
	vtkSmartPointer<vtkCPProcessor> Processor;
	void init();
	int wholeExtents[6];

	std::stringstream log;

  public:
	CatalystAdaptor();
	CatalystAdaptor(int numScripts, char* scripts[]);
	CatalystAdaptor(int numScripts, std::vector<std::string>scripts);

	void finalize();

	void setWholeExtents(int minX, int maxX,  int minY, int maxY,  int minZ, int maxZ);
	int addPipelines(int numScripts, std::string scripts[]);

	void coProcess(vtkDataObject *grid, double time, unsigned int timeStep, bool lastTimeStep);
	void coProcess(vtkUnstructuredGrid *grid, double time, unsigned int timeStep, bool lastTimeStep);

	std::string getLog(){ return log.str(); }
};



inline CatalystAdaptor::CatalystAdaptor()
{
	Processor = NULL;
	init();
}


inline CatalystAdaptor::CatalystAdaptor(int numScripts, char* scripts[])
{

	std::vector <std::string> scriptsArray;
	for (int i=0; i<numScripts; i++)
		scriptsArray.push_back( scripts[i] );

	Processor = NULL;
	init();
	addPipelines(numScripts, &scriptsArray[0]);
}


inline CatalystAdaptor::CatalystAdaptor(int numScripts, std::vector<std::string>scripts)
{

	Processor = NULL;
	init();
	addPipelines(numScripts, &scripts[0]);
}



inline void CatalystAdaptor::init()
{
	if (Processor == NULL)
	{
		std::cout << "Hi" << std::endl;
		Processor = vtkSmartPointer<vtkCPProcessor>::New();
		Processor->Initialize();
		std::cout << "Jo" << std::endl;
	}
	else
		Processor->RemoveAllPipelines();
}


inline int CatalystAdaptor::addPipelines(int numScripts, std::string scripts[])
{

	int numValidScripts = 0;
	int success = 1;
	for (int i=0; i<numScripts; i++)
	{
		// Skipping files that do not end in .py
		if ( !( scripts[i].substr( scripts[i].size()-3,3 ) == ".py" ) )
			continue;

		// Check if the file exist
		std::ifstream inputFile(scripts[i].c_str());
		if (!inputFile)
			continue;

		vtkNew<vtkCPPythonScriptPipeline> pipeline;
		if (pipeline->Initialize(scripts[i].c_str()) )
		{
			Processor->AddPipeline(pipeline.GetPointer());
			numValidScripts++;
		}
	}


	//std::cout << "numValidScripts: " << numValidScripts << std::endl;
	return numValidScripts++;
}


inline void CatalystAdaptor::setWholeExtents(int minX, int maxX, int minY, int maxY, int minZ, int maxZ)
{
	wholeExtents[0] = minX; 	wholeExtents[1] = maxX;
	wholeExtents[2] = minY; 	wholeExtents[3] = maxY;
	wholeExtents[4] = minZ; 	wholeExtents[5] = maxZ;
}


inline void CatalystAdaptor::finalize()
{
	Processor->Finalize();
	if (Processor)
		Processor = NULL;
}


inline void CatalystAdaptor::coProcess(vtkDataObject *grid, double time, unsigned int timeStep, bool lastTimeStep)
{
	vtkNew<vtkCPDataDescription> dataDescription;
	dataDescription->AddInput("input");
	dataDescription->SetTimeData(time, timeStep);

	// Sssume that we want to all the pipelines to execute if it is the last time step.
	if (lastTimeStep == true)
	{
		dataDescription->ForceOutputOn();
	}

	if (Processor->RequestDataDescription(dataDescription.GetPointer()) != 0)
	{
		dataDescription->GetInputDescriptionByName("input")->SetGrid(grid);
		Processor->CoProcess(dataDescription.GetPointer());
	}
}


inline void CatalystAdaptor::coProcess(vtkUnstructuredGrid  *uGrid, double time, unsigned int timeStep, bool lastTimeStep)
{

	vtkNew<vtkCPDataDescription> dataDescription;
	dataDescription->AddInput("input");
	dataDescription->SetTimeData(time, timeStep);

	// Sssume that we want to all the pipelines to execute if it is the last time step.
	if (lastTimeStep == true)
	{
		dataDescription->ForceOutputOn();
		
	}

	if (Processor->RequestDataDescription(dataDescription.GetPointer()) != 0)
	{
		//std::cout << "CatalystAdaptor::coProcess "  << std::endl;

		dataDescription->GetInputDescriptionByName("input")->SetGrid(uGrid);
		Processor->CoProcess(dataDescription.GetPointer());
	}
}

