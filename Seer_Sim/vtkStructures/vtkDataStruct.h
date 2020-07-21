#pragma once

#include <string>
#include <vtkSmartPointer.h>
#include <vtkDataSet.h>


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