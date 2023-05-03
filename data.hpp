#pragma once

#include <string>
#include <map>
#include <vector>
#include <sstream>

#include "serialize.hpp"



struct Record
{
    std::map<std::string, std::string> info;
    char *buffer;
    std::string serializedData;

    std::string serializeInfo()
    {
        std::stringstream ss;
        for (auto i : info)
            ss << i.first << ":" << i.second << ";";
        
        return ss.str();
    }

    template <typename T> 
    int setData(T* data, int numPoints)
    {
        std::stringstream ss;
        for (size_t i=0; i<numPoints-1; i++)
            ss << data[i] << ",";
        ss << data[numPoints-1] = data[numPoints-1];
        serializedData = ss.str();

        return 0;
    }
};


class SimData
{
  public:
    std::vector<Record> data;
    
    SimData(){};
    ~SimData(){};

    int send();

};

int SimData::send()
{
    std::string dataToSend = "";

    // Serialize the data
    for (int i=0; i<data.size(); i++)
    {
        // Serialize map
        std::string recordString;
        std::string metadata = data[i].serializeInfo();
        std::string simData = data[i].serializedData;
        recordString = metadata + "||" + simData + "**||";

        dataToSend += recordString;
    }



}

/*
info:
- type: float | int | double | ...
- association: vertex | cell | edge | face
- arrangement: scalar | vector | tensor
- elem-size: 1 | 2 | 3 | ...
- total-num-elems: n

*/