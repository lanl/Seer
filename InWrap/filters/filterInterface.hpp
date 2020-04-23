/*================================================================================
This software is open source software available under the BSD-3 license.
Copyright (c) 2019. Triad National Security, LLC.
All rights reserved.
Authors:
 - Pascal Grosset
================================================================================*/

#pragma once

#include <vector>
#include <string>
#include <sstream>

#include <mpi.h>

#include "log.hpp"
#include "timer.hpp"
#include "memory.hpp"
#include "vtkDataStruct.h"


struct paramInfo
{
	std::string dataType;
	std::string value;

	paramInfo(){};
	
	paramInfo(std::string type, std::string val)
	{
		dataType = type;
		value = val;
	}

	template<class T>
	T getValue()
	{
		std::stringstream sstr(value);
		T val;
		sstr >> val;
		return val;
	}
};


class FilterInterface
{
  protected:
	int myRank;
	int numRanks;
	MPI_Comm comm; 

	std::string name;
	std::string scope;		 	// affect whole data or a scalar | whole, single
	std::string outputType; 	// single, array, dataset

	std::map<std::string, paramInfo> varValue;  // used for output or more inputs are needed


  public:
	void *outputData;   // if a filter needs to return a data array

	virtual void init(MPI_Comm mpiComm) = 0;
	virtual int execute(InWrap::VTKDataStruct *simData) = 0;
	virtual void close() = 0;

	void insertParam(std::string name, std::string dataType, std::string value);
	paramInfo getParamInfo(std::string name);
	template<class T> T getParamValue(std::string name);
	std::string getName();
};



inline void FilterInterface::insertParam(std::string name, std::string dataType, std::string value)
{
	varValue[name] = paramInfo(dataType, value);
}


inline paramInfo FilterInterface::getParamInfo(std::string name)
{
	return varValue[name];
}


template<class T>
inline T FilterInterface::getParamValue(std::string name)
{
	return (varValue[name]).value;
}


inline std::string FilterInterface::getName()
{ 
	return name; 
}
