#pragma once

#include<stdio.h> 
#include<stdbool.h> 
#include <string>
#include <fstream>
#include <iostream>

namespace InWrap  
{ 

inline bool fileExisits(std::string filename) 
{
    std::ifstream ifs(filename.c_str());
    return ifs.good();
}

}