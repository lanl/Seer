#pragma once

#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <cstring>

#include "utility.hpp"


struct RecvData
{
	std::map<std::string, std::string> header;
    void * data;
};


class Receiver
{
    std::vector<RecvData> recvData;

    std::map<std::string, std::string> deserializeMap(char * buff, size_t headerSize);

  public:
    Receiver(){};
    ~Receiver(){};

    void clean();

    int receiveData(char *buffer, size_t dataSize);
};


inline void Receiver::clean()
{

}



inline std::map<std::string, std::string> Receiver::deserializeMap(char * buff, size_t headerSize)
{
    std::string tempStr = std::string(buff, headerSize); 
    std::stringstream ss(tempStr);  

    std::map<std::string, std::string> header;
    std::string word;
    while (std::getline(ss, word)) { // Extract words from the stream.
        size_t found = word.find(':');
        if (found!=std::string::npos)
        {
            std::string key = word.substr(0,found); 
            header[key] = word.substr(found+1,word.length());
            std::cout <<  key << ": " << header[key] << std::endl;
        }
    }

    return header;
}


int Receiver::receiveData(char *recvBuffer, size_t dataSize)
{
    size_t offset = 0;
    int numRecords = 0;
    std::vector<int> dataSizes;


    // Get number of records
    memcpy(&numRecords, recvBuffer+offset, sizeof(int));    offset += sizeof(int);
    dataSizes.resize(numRecords);


    // Get the size of each record
    for (int i=0; i<numRecords; i++)
    {
        memcpy(&dataSizes[i], recvBuffer+offset, sizeof(int));  offset += sizeof(int);
    }

    

    // Get each record
    for (int r=0; r<numRecords; r++)
    {
        RecvData tempRec;

        int headerSize;
        memcpy(&headerSize, recvBuffer+offset, sizeof(int));    offset += sizeof(int);


        // Get header
        char *headerBuffer = new char[headerSize];
        memcpy(headerBuffer, recvBuffer+offset, headerSize);   offset += headerSize;
        tempRec.header = deserializeMap(headerBuffer, headerSize);


        // Get data
        int dataSize = std::stoi(tempRec.header["size"]) * getTypeSize(tempRec.header["type"]);
        allocateMemory(tempRec.data, tempRec.header["type"], std::stoi(tempRec.header["size"]));
        memcpy(tempRec.data, recvBuffer+offset, dataSize);     offset += dataSize;


       
        // Testing
        // for (int i=0; i<std::stoi(tempRec.header["size"]); i++)
        //     std::cout << static_cast<float *>(tempRec.data)[i] << std::endl;


        float * tempData = new float[ stoi(tempRec.header["size"]) ];
        memcpy(tempData, tempRec.data, stoi(tempRec.header["size"])*sizeof(float));
        for (int i=0; i<stoi(tempRec.header["size"]); i++)
            std::cout << tempData[i] << ", ";
        std::cout << "\n";


        recvData.push_back(tempRec);

        delete []headerBuffer;
    }

    return 1;
}