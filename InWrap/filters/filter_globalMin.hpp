/*================================================================================
This software is open source software available under the BSD-3 license.
Copyright (c) 2019. Triad National Security, LLC.
All rights reserved.
Authors:
 - Pascal Grosset
================================================================================*/

#pragma once

#include <limits>       
#include <algorithm>
#include <string>

#include "filterInterface.hpp"


class GlobalMin : public FilterInterface
{
	// output of filter is in dictionary as
	// varValue["global_min"]

  public:
  	GlobalMin();
  	~GlobalMin(){};

	void init(MPI_Comm mpiComm);
    int execute(InWrap::VTKDataStruct *simData);
    void close(){};
};


inline GlobalMin::GlobalMin()
{
	myRank = 0;
	numRanks = 0;
	name = "global_min";
	scope = "local";
	outputType = "single";
}


inline void GlobalMin::init(MPI_Comm mpiComm)
{
	comm = mpiComm;
	MPI_Comm_rank(comm, &myRank);
	MPI_Comm_size(comm, &numRanks);
}


inline int GlobalMin::execute(InWrap::VTKDataStruct *simData)
{
	InWrap::Timer clock("filter");

	/*
	double localMin = std::numeric_limits<double>::max();

	// Compute local minumum
  #ifdef _OPENMP
	#pragma omp parallel for collapse(2)
  #endif
	for (int d=0; d<numDims; d++)
		for (size_t i=0; i<n[d]; i++)
		{
			if (dataType == "float")
				localMin = std::min( (static_cast<float *>(data)[i]), (float)localMin );
			else if (dataType == "double")
				localMin = std::min( (static_cast<double *>(data)[i]), (double)localMin );
			else if (dataType == "int")
				localMin = std::min( (static_cast<int *>(data)[i]), (int)localMin );
			else
				return -1;
		}

	// Compute global min
	double globalMin;
	MPI_Reduce(&localMin, &globalMin, 1, MPI_DOUBLE, MPI_MIN, 0, MPI_COMM_WORLD);

	// store value in dictionary
	varValue["global_min"] = paramInfo("double", std::to_string(globalMin));
	*/

	clock.stop("filter");
	debugLog << "global_min filter took " << clock.getDuration("filter") << " s" << std::endl;

	return 1;
}