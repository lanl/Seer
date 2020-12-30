#pragma once

#include <string>
#include <vtkDataSet.h>
#include <vtkSmartPointer.h>


namespace Seer
{


class VTKDataStruct
{
	std::string type;

  public:
  	virtual vtkSmartPointer<vtkDataSet> getGrid() = 0;

  	std::string getType(){ return(type); }
};

} //InWrap