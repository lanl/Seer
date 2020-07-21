#pragma once

#ifdef CATALYST_ENABLED


#include <vtkNew.h>
#include <vtkSmartPointer.h>
#include <vtkCPDataDescription.h>
#include <vtkCPInputDataDescription.h>
#include <vtkCPProcessor.h>
#include <vtkCPPythonScriptPipeline.h>
#include <vtkMPI.h>
#include <vtkDataObject.h>


#include "unstructuredGrid.h"
#include "structuredGrid.h"
#include "rectilinearGrid.h"

#include "argsParser.hpp"
#include "timer.hpp"

#include <mpi.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>


namespace Seer
{


inline bool isPythonFile(std::string filename)
{
	// Skip if file that does not end in .py
	if ( !( filename.substr( filename.size()-3,3 ) == ".py" ) )
		return false;

	// skip if the file does not exist
	std::ifstream inputFile(filename.c_str());
	if (!inputFile)
		return false;

	return true;
}


inline std::vector <std::string> catalystScriptChecker(int numArgs, char *argv[])
{
	std::vector <std::string> catalystScripts;


	parseArgs(numArgs, argv);

	int numInputCatalystScripts = getNumMatchingArgs("--catalyst");

	
	if (numInputCatalystScripts > 0)
	{
		int catalystParamFound = 0;
		for (int i=0; i<numInputCatalystScripts; i++)
		{
			std::string scriptName;
			catalystParamFound = findArg("--catalyst", catalystParamFound);
			if (! getArgument(catalystParamFound+1,scriptName) )
				continue;

			if (!isPythonFile(scriptName))
				continue;

			catalystScripts.push_back(scriptName);
		}
	}
	

	return catalystScripts;
}



class CatalystAdaptor
{
	vtkSmartPointer<vtkCPProcessor> Processor;
	void init();
	int wholeExtents[6];

	std::stringstream log;

  public:
	CatalystAdaptor();
	CatalystAdaptor(int numScripts, char* scripts[]);
	CatalystAdaptor(int numScripts, std::vector<std::string> scripts);

	void finalize();

	void setWholeExtents(int minX, int maxX,  int minY, int maxY,  int minZ, int maxZ);
	int addPipelines(int numScripts, std::vector<std::string> scripts);

	void init(int numScripts, std::vector<std::string> scripts);

	void coProcess(vtkDataObject *grid, double time, unsigned int timeStep, bool lastTimeStep);
	void coProcess(vtkUnstructuredGrid *grid, double time, unsigned int timeStep, bool lastTimeStep);
	void coProcess(vtkStructuredGrid   *grid, double time, unsigned int timeStep, bool lastTimeStep);
	void coProcess(vtkRectilinearGrid  *grid, double time, unsigned int timeStep, bool lastTimeStep);

	
	std::string getLog(){ return log.str(); }
};


inline CatalystAdaptor::CatalystAdaptor()
{
	Processor = NULL;
	init();
}


inline CatalystAdaptor::CatalystAdaptor(int numScripts, char* scripts[])
{
	Timer clock;
	clock.start("constructor");

	std::vector <std::string> scriptsArray;
	for (int i=0; i<numScripts; i++)
		scriptsArray.push_back( scripts[i] );

	Processor = NULL;
	init();
	addPipelines(numScripts, scriptsArray);

	clock.stop("constructor");
	log << "CatalystAdaptor Constructor took: " << clock.getDuration("constructor") << " s" << std::endl;
}


inline CatalystAdaptor::CatalystAdaptor(int numScripts, std::vector<std::string> scripts)
{
	Timer clock;
	clock.start("constructor");

	Processor = NULL;
	init();
	addPipelines(numScripts, scripts);

	clock.stop("constructor");
	log << "CatalystAdaptor Constructor took: " << clock.getDuration("constructor") << " s" << std::endl;
}


inline void CatalystAdaptor::init(int numScripts, std::vector<std::string> scripts)
{
	Timer clock;
	clock.start("init");

	init();
	for (int i=0; i<numScripts; i++)
	{
		std::cout << scripts[i] << std::endl;
	}
	addPipelines(numScripts, scripts);

	clock.stop("init");
	log << "CatalystAdaptor Constructor took: " << clock.getDuration("init") << " s" << std::endl;
}


inline void CatalystAdaptor::init()
{
	Timer clock;
	clock.start("init");


	if (Processor == NULL)
	{
		Processor = vtkSmartPointer<vtkCPProcessor>::New();
		Processor->Initialize();
	}
	else
	{
		Processor->RemoveAllPipelines();
	}

	clock.stop("init");
	log << "CatalystAdaptor init took: " << clock.getDuration("init") << " s" << std::endl;
}


inline int CatalystAdaptor::addPipelines(int numScripts, std::vector<std::string> scripts)
{
	Timer clock;
	clock.start("addPipelines");

	int numValidScripts = 0;
	int success = 1;
	for (int i=0; i<numScripts; i++)
	{
		// Skipping files that do not end in .py
		if ( !( scripts[i].substr( scripts[i].size()-3,3 ) == ".py" ) )
		{
			std::cout << "scripts[i]" << scripts[i] << "is invalid" <<std::endl;
			continue;
		}

		// Check if the file exist
		std::ifstream inputFile(scripts[i].c_str());
		if (!inputFile)
		{
			std::cout << "scripts[i]" << scripts[i] << "does not exist" <<std::endl;
			continue;
		}


		std::cout << "before initialize" << scripts[i] <<std::endl;
		vtkNew<vtkCPPythonScriptPipeline> pipeline;
		if (pipeline->Initialize(scripts[i].c_str()) )
		{
			Processor->AddPipeline(pipeline.GetPointer());
			numValidScripts++;
			std::cout << "scripts[i]" << scripts[i] << "added to pipeline" <<std::endl;
		}
		else
		{
			log << "scripts[i]" << scripts[i] << "could not be added to pipeline" <<std::endl;
		}
	}

	clock.stop("addPipelines");
	log << "CatalystAdaptor Adding pipelines took: " << clock.getDuration("addPipelines") << " s" << std::endl;

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
	Processor->RemoveAllPipelines();

	Processor->Finalize();
	if (Processor)
		Processor = NULL;
}


inline void CatalystAdaptor::coProcess(vtkDataObject *grid, double time, unsigned int timeStep, bool lastTimeStep)
{
	Timer clock;
	clock.start("coProcess");

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

	clock.stop("coProcess");
	log << "CatalystAdaptor coProcess: " << clock.getDuration("coProcess") << " s" << std::endl;
}


inline void CatalystAdaptor::coProcess(vtkUnstructuredGrid  *uGrid, double time, unsigned int timeStep, bool lastTimeStep)
{
	Timer clock;
	clock.start("coProcess");

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
		dataDescription->GetInputDescriptionByName("input")->SetGrid(uGrid);
		Processor->CoProcess(dataDescription.GetPointer());
	}

	clock.stop("coProcess");
	log << "CatalystAdaptor coProcess: " << clock.getDuration("coProcess") << " s" << std::endl;
}


inline void CatalystAdaptor::coProcess(vtkStructuredGrid  *grid, double time, unsigned int timeStep, bool lastTimeStep)
{
	Timer clock;
	clock.start("coProcess");

	vtkNew<vtkCPDataDescription> dataDescription;

	dataDescription->AddInput("input");
	dataDescription->SetTimeData(time, timeStep);
	

	// Sssume that we want to all the pipelines to execute if it is the last time step.
	if (lastTimeStep == true)
		dataDescription->ForceOutputOn();

	if (Processor->RequestDataDescription(dataDescription.GetPointer()) != 0)
	{
		dataDescription->GetInputDescriptionByName("input")->SetWholeExtent(wholeExtents[0], wholeExtents[1],
																				wholeExtents[2], wholeExtents[3],
																				wholeExtents[4], wholeExtents[5]);
		dataDescription->GetInputDescriptionByName("input")->SetGrid(grid);
		Processor->CoProcess(dataDescription.GetPointer());
	}

	clock.stop("coProcess");
	log << "CatalystAdaptor coProcess: " << clock.getDuration("coProcess") << " s" << std::endl;
}


inline void CatalystAdaptor::coProcess(vtkRectilinearGrid  *recGrid, double time, unsigned int timeStep, bool lastTimeStep)
{
	Timer clock;
	clock.start("coProcess");

	vtkNew<vtkCPDataDescription> dataDescription;
	dataDescription->AddInput("input");
	dataDescription->SetTimeData(time, timeStep);

	// Assume that we want to all the pipelines to execute if it is the last time step.
	if (lastTimeStep == true)
		dataDescription->ForceOutputOn();

	if (Processor->RequestDataDescription(dataDescription.GetPointer()) != 0)
	{
		dataDescription->GetInputDescriptionByName("input")->SetGrid(recGrid);
		Processor->CoProcess(dataDescription.GetPointer());
	}

	clock.stop("coProcess");
	log << "CatalystAdaptor coProcess: " << clock.getDuration("coProcess") << " s" << std::endl;
}

} // namespace seer

#endif