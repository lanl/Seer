#pragma once

#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <cstring>

#include <thallium.hpp>
#include "utility.hpp"

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
    std::vector<Record> records;

    int myRank;
    int numRanks;
    std::string serverIp;
    int serverPort;


    char * serializeMap(std::map<std::string, std::string> header, int &headerSize);
    void clean();
    void sendRDMA(char * msg1, int msg1Len, char * msg2, int msg2Len);

  public:
    Sender(){};
    ~Sender(){ clean(); };

    void init(std::string _serverIp, int _serverPort, int _myRank, int _numRanks);

    template <typename T>
    int setData(std::map<std::string, std::string> header, T* data, size_t numElements);
    int sendData(int ts);
};



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


inline void Sender::clean()
{
    for (int r=0; r<records.size(); r++)
        records[r].clean();
}


inline void Sender::sendRDMA(char * msg1, int msg1Len, char * msg2, int msg2Len)
{
    thallium::engine myEngine("tcp", MARGO_CLIENT_MODE);
    thallium::remote_procedure remote_do_rdma = myEngine.define("do_rdma");

    std::string serverAddress = "tcp://" + serverIp + ":" + std::to_string(serverPort);
    thallium::endpoint server_endpoint = myEngine.lookup(serverAddress);

    std::vector<std::pair<void*,std::size_t>> segments(2);
    segments[0].first  = msg1;
    segments[0].second = msg1Len;

    segments[1].first  = msg2;
    segments[1].second = msg2Len;

    thallium::bulk myBulk = myEngine.expose(segments, thallium::bulk_mode::read_only);

    remote_do_rdma.on(server_endpoint)(myBulk);
}






inline void Sender::init(std::string _serverIp, int _serverPort, int _myRank, int _numRanks)
{
    serverIp = _serverIp;
    serverPort = _serverPort;
    numRanks = _numRanks;
    myRank = _myRank;
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


inline int Sender::sendData(int ts)
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
    int bufferPart1Size = sizeof(int)*4;
    int buff1Offset = 0;
    char * bufferPart1 = new char[ bufferPart1Size ];
    memcpy(bufferPart1 + buff1Offset, &buffer2Offset,   sizeof(int)); buff1Offset += sizeof(int); // size of the data
    memcpy(bufferPart1 + buff1Offset, &myRank,          sizeof(int)); buff1Offset += sizeof(int); // myRank    
    memcpy(bufferPart1 + buff1Offset, &numRanks,        sizeof(int)); buff1Offset += sizeof(int); // numRanks
    memcpy(bufferPart1 + buff1Offset, &ts,              sizeof(int)); buff1Offset += sizeof(int); // timestep


    //
    // Do the send !!!
    //
    sendRDMA(bufferPart1, bufferPart1Size, bufferPart2, totalSize);

   

    // clean up; release memory
    delete [] bufferPart1;
    delete [] bufferPart2;

    clean();

    return 1;
}