/*================================================================================
This software is open source software available under the BSD-3 license.
Copyright (c) 2019. Triad National Security, LLC.
All rights reserved.
Authors:
 - Pascal Grosset
================================================================================*/

#pragma once

//#ifdef HAVE_VTK

#include "unstructuredGrid.h"

#include <vtkProgrammableFilter.h>
#include <vtkSOADataArrayTemplate.h>

#include "filterInterface.hpp"


class SeerVTKOffset : public FilterInterface
{
	// output of filter is in dictionary as
	// varValue["seer_vtkoffset"]

  public:
  	SeerVTKOffset();
  	~SeerVTKOffset(){};

	void init(MPI_Comm mpiComm);
    int execute(void *data, std::string dataType, int numDims, size_t *n);
    void close(){};
};


inline void SeerVTKOffset::GlobalMin()
{
	myRank = 0;
	numRanks = 0;
	filterName = "global_min";
}


inline void SeerVTKOffset::init(MPI_Comm mpiComm)
{
	comm = _comm;
	MPI_Comm_rank(comm, &myRank);
	MPI_Comm_size(comm, &numRanks);
}


inline int SeerVTKOffset::execute(void *data, std::string dataType, int numDims, size_t *n)
{
	Timer clock("filter");

	// Convert the data to VTK
	size_t numPoint = 0;
	for (int d=0; d<numDims; d++)
		numPoint += n[d];

	vtkSOADataArrayTemplate<T>* temp = vtkSOADataArrayTemplate<T>::New();
	
	temp->SetNumberOfTuples(numPoints);
	temp->SetNumberOfComponents(1);
	temp->SetName(varName.c_str());
	temp->SetArray(0, data, numPoints, false, true);
	strucGrid->GetPointData()->AddArray(temp);

	// Apply Filter
	

	clock.stop("filter");
	debugLog << "global_min filter took " << clock.getDuration("filter") << " s" << std::endl;

	return 1;
}

//#endif //HAVE_VTK

