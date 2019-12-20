#pragma once

#ifdef SENSEI_ENABLED

#include <stdio.h>
#include <string.h>

#include <ConfigurableAnalysis.h>
#include <VTKDataAdaptor.h>
#include <PythonAnalysis.h>

#include "vtkUnstructuredGrid.h"
#include "vtkStructuredGrid.h"
#include "vtkRectilinearGrid.h"

#include <vtkSmartPointer.h>
#include <vtkStructuredGrid.h>

namespace InWrap  
{ 

static vtkSmartPointer<sensei::VTKDataAdaptor> dataAdaptor;
static vtkSmartPointer<sensei::ConfigurableAnalysis> analysisAdaptor;

inline bool checkSensei(char *arg)
{
	if ( strcmp(arg, "--sensei") == 0 )
		return true;

	return false;
}


inline void senseiInitialize(MPI_Comm _comm, std::string configFile)
{
	dataAdaptor = vtkSmartPointer<sensei::VTKDataAdaptor>::New();
	dataAdaptor->SetCommunicator(_comm);

	analysisAdaptor = vtkSmartPointer<sensei::ConfigurableAnalysis>::New();
	analysisAdaptor->SetCommunicator(_comm);
  	analysisAdaptor->Initialize(configFile);
}


inline void senseiAnalyze(std::string name, vtkSmartPointer<vtkDataSet> grid, unsigned int timeStep, float time)
{
	dataAdaptor->SetDataTime(time);
	dataAdaptor->SetDataTimeStep(timeStep);
	dataAdaptor->SetDataObject(name, grid);

	analysisAdaptor->Execute(dataAdaptor.GetPointer());

	dataAdaptor->ReleaseData();
}

inline void senseiFinalize()
{
	dataAdaptor = nullptr;
	analysisAdaptor->Finalize();
	analysisAdaptor = nullptr;
}


}

#endif // SENSEI_ENABLE