#pragma once

#include<stdio.h> 
#include<stdbool.h> 
#include <string>
#include <fstream>
#include <iostream>

namespace Seer  
{ 

inline bool fileExisits(std::string filename) 
{
    std::ifstream ifs(filename.c_str());
    return ifs.good();
}


template <class T>
std::string serializeArray(size_t n, T *array)
{
    std::cout << "n: " << n << std::endl;

    std::stringstream ss;
    for (size_t i=0; i<n; i++)
    {
        ss << array[i] << ",";
        //std::cout << array[i] << std::endl;

    }

    return ss.str();
}


// Does a uniform sampling of particles
template <class T>
std::vector<T> randomSamplePoints(size_t n, float samplingRate, T *dataIn)
{
    std::cout << "n: " << n << ", samplingRate: " << samplingRate << std::endl;
	// Reserve space for new output
	size_t numSampledPoints = samplingRate * n;
    std::vector<T> dataOut;
	//dataOut = new T[numSampledPoints];

    int sampleAt = n/numSampledPoints;

    std::cout << "sampleAt: " << sampleAt << std::endl;

	// Allocate data
	size_t index = 0;
	for (size_t i=0; i<n; i++)
	{
		if (i%sampleAt == 0)
		{
			//dataOut[index] = dataIn[i];
            dataOut.push_back(dataIn[i]);
			index++;

			// Prevent overflow
			if (index == numSampledPoints)
				break;
		}
	}
    return dataOut;

    // std::string outputStr = serializeArray(numSampledPoints, dataOut);

    // // std::stringstream ss;
    // // for (size_t i=0; i<numSampledPoints-1; i++)
    // // {
    // //     ss << dataOut[i] << ",";
    // // }
    // std::cout << outputStr << std::endl;
}





// average values for each 3D subvolume
template <class T>
void downSampleGrid(size_t dims[3], float samplingRate, size_t newDims[3], T *dataIn, T *dataOut)
{
    // Reserve space for new output
    size_t numSampledPointsX = samplingRate * dims[0];
    size_t numSampledPointsY = samplingRate * dims[1];
    size_t numSampledPointsZ = samplingRate * dims[2];
    dataOut = new T[numSampledPointsX*numSampledPointsY*numSampledPointsZ];

    // Fill using subvolume
	size_t sampleAtX = float(dims[0])/sampleAtX;
    size_t sampleAtY = float(dims[1])/sampleAtY;
    size_t sampleAtZ = float(dims[2])/sampleAtZ;


    size_t numSamplePoints = numSampledPointsX * numSampledPointsY * numSampledPointsZ;

    size_t index = 0;
    for (size_t i=0; i<numSamplePoints; i++)
    {
        
        //size_t localIndex = 
    }
}


} // Namespcase Seer