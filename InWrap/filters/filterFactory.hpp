/*================================================================================
This software is open source software available under the BSD-3 license.
Copyright (c) 2017, Los Alamos National Security, LLC.
All rights reserved.
Authors:
 - Pascal Grosset
================================================================================*/

#pragma once

#include "filterIncludes.h"
#include "filterInterface.hpp"

class FilterFactory
{
  public:
	static FilterInterface * createFilter(std::string filterName)
	{
		return NULL;
	}
};