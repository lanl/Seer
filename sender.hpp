#pragma once

#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <cstring>

#include "utility.hpp"
#include "rpc.hpp"

// Msg: part 1
// # records
// whole data size

// Msg: part 2
// num records
// data 1 size
// data 2 size
//      .
//      .
// data n size
// data 1
//    header 1 size
//    data 1
// data 2
//    header 2 size
//    data 2
//      .
//      .
// data n
//      header n size
//      data n


struct Record
{
    char * header;
    char * data;
    int headerSize;
    int dataSize;

    Record()
    {
        header = NULL;
        data = NULL;

        dataSize = 0;
        headerSize = 0;
    };

    void clean(){
        if (header != NULL)
            delete []header;

        if (data != NULL)
            delete []data;

        dataSize = 0;
        headerSize = 0;

        header = NULL;
        data = NULL;
    };
};



class Sender
{
    RPC rpc;
    std::vector<Record> records;

    int myRank;
    int numRanks;

    char * serializeMap(std::map<std::string, std::string> header, int &headerSize);
    void clean();

  public:
    Sender(){};
    ~Sender(){ clean(); };

    void init(std::string rpcMode, std::string serverIp, int serverPort);

    template <typename T>
    int setData(std::map<std::string, std::string> header, T* data, size_t numElements);
    //char * sendData(int &dataSize);
    int sendData();
};


inline void Sender::clean()
{
    for (int r=0; r<records.size(); r++)
        records[r].clean();
}


inline void Sender::init(std::string rpcMode, std::string serverIp, int serverPort, int _myRank, int _numRanks)
{
    rpc.init(rpcMode, serverIp, serverPort);
    numRanks = _numRanks;
    myRank = _myRank;
}


inline char * Sender::serializeMap(std::map<std::string, std::string> header, int &headerSize)
{
    std::stringstream ss;
    for (auto const& i : header)
        ss << i.first << ":" << i.second << "\n";
    
    std::string tempStr = ss.str();
    headerSize = tempStr.length()+1;

    char * tempBuf = new char[headerSize];
    strcpy(tempBuf, tempStr.c_str());

    return tempBuf;
}


template <typename T>
inline int Sender::setData(std::map<std::string, std::string> header, T* data, size_t numElements)
{
    Record rec;

    // Header
    rec.header = serializeMap(header, rec.headerSize);


    // Data
    rec.dataSize = numElements * getTypeSize(header["type"]);  
    rec.data = new char[ rec.dataSize ];
    memcpy(rec.data, &data[0], rec.dataSize );


    //std::cout << "\nnumElements: " << numElements << ", getTypeSize(header[type]): " << getTypeSize(header["type"]) << std::endl;
    //std::cout << "data size: " << rec.dataSize << ", header size: " << rec.headerSize << std::endl; 

    records.push_back(rec);

    return 1;
}


inline int Sender::sendData()
//inline char * Sender::sendData(int &dataSize)
{
    // Calculate total size of the data
    int totalSize = 0;
    totalSize += sizeof(int);                   // number of messages
    totalSize += sizeof(int) * records.size();  // size of each message
    for (int r=0; r<records.size(); r++)        // each message
        totalSize += (sizeof(int)  + records[r].headerSize + records[r].dataSize);


    // Create part 2 of the buffer
    int buffer2Offset = 0;
    char * bufferPart2 = new char[totalSize];   


    // Put # records
    int numRecords = records.size();
    memcpy(bufferPart2 + buffer2Offset, &numRecords, sizeof(int));  buffer2Offset += sizeof(int);   

    // Put each record size
    for (int n=0; n<numRecords; n++)
    {
        int recordSize = sizeof(int) + records[n].headerSize + records[n].dataSize;
        memcpy(bufferPart2 + buffer2Offset, &recordSize, sizeof(int));  buffer2Offset += sizeof(int);
    }

    
    // Put the actual data
    for (int r=0; r<records.size(); r++)
    {
        memcpy(bufferPart2 + buffer2Offset, &records[r].headerSize, sizeof(int));           buffer2Offset += sizeof(int);
        memcpy(bufferPart2 + buffer2Offset, records[r].header,      records[r].headerSize); buffer2Offset += records[r].headerSize;
        memcpy(bufferPart2 + buffer2Offset, records[r].data,        records[r].dataSize);   buffer2Offset += records[r].dataSize;
    }


    // Test
    void * data;
    allocateMemory(data, "float", 8);
    std::memcpy(data, bufferPart2 + (sizeof(int) + records[0].headerSize), records[0].dataSize);

    for (int i=0; i<8; i++)
        std::cout << static_cast<float *>(data)[i] << std::endl;



    //
    // Potentially compress the data here!!!
    // 


    // Create part 1 of the buffer
    int bufferPart1Size = sizeof(int)*3;
    int buff1Offset = 0;
    char * bufferPart1 = new char[ bufferPart1Size ];
    memcpy(bufferPart1 + buff1Offset, &buffer2Offset,   sizeof(int)); buff1Offset += sizeof(int); // size of the data
    memcpy(bufferPart1 + buff1Offset, &myRank,          sizeof(int)); buff1Offset += sizeof(int); // myRank    
    memcpy(bufferPart1 + buff1Offset, &numRanks,        sizeof(int)); buff1Offset += sizeof(int); // numRanks


    //
    // Do the send !!!
    //
    rpc.clientSend(bufferPart1, bufferPart1Size, bufferPart2, totalSize);

   

    // clean up; release memory
    delete [] bufferPart1;
    delete [] bufferPart2;

    clean();

    return 1;

    // dataSize = totalSize;
    // return bufferPart2;
}