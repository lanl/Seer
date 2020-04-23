#pragma once

#include <vector>
#include <string>
#include <map>


struct variable
{
	std::string name;
	std::string dataType;
	size_t numElements;
	void *data;
	std::string topo; // cell or vertex for grid based data

	variable(std::string _name, std::string _dataType, size_t _numElements, void *_data , std::string _topo="")
	{
		name = _name;
		dataType = _dataType;
		numElements = _numElements;
		data = _data;
		topo = _topo;
	}
};


struct simData
{
	std::string type; // unstructured_grid, structured_grid
	int numDims;
	std::vector<int> dims;
	std::vector<float> realDims;

	std::map<std::string, variable> variables;

	simData(std::string _type, int _numDims, int _dims[])
	{
		type = _type;
		numDims = _numDims;
		for (auto i=0; i<_numDims; i++)
			dims.push_back(dims[i]);
	}
};