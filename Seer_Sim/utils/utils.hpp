#pragma once

#include<stdio.h> 
#include<stdbool.h> 
#include <string>
#include <fstream>
#include <iostream>     // std::cout
#include <algorithm>    // std::shuffle
#include <array>        // std::array
#include <random>       // std::default_random_engine
#include <chrono>       // std::chrono::system_clock

namespace Seer  
{ 

inline bool fileExisits(std::string filename) 
{
    std::ifstream ifs(filename.c_str());
    return ifs.good();
}


std::vector<int> shuffleArray(int numPoints)
{
    std::vector<int> indices;
    indices.resize(numPoints);
    for (int i=0; i<numPoints; i++)
        indices[i] = i;

    auto rng = std::default_random_engine {};
    std::shuffle(std::begin(indices), std::end(indices), rng);

    return indices;
}

std::string serializeArray(float *data, int numPoints, float percentage, std::vector<int> indices)
{
    std::string serializedData = "";
    int numSamplePoints = (int) (percentage*numPoints);


    int i=0;
    for (i=0; i<numSamplePoints-1; i++)
        serializedData += std::to_string(data[ indices[i] ]) + ",";
    serializedData += std::to_string(data[ indices[i] ]);

    return serializedData;
}

} // Namespcase Seer