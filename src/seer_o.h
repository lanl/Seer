#pragma once

#include <sstream>
#include <string>
#include <iostream>
#include <fstream>
#include <cassert>
#include <algorithm>
#include <vector>
#include <map>
#include <cstdlib>

#include <time.h> 


#include <nlohmann/json.hpp>

#include <yokan/database.h>
#include <yokan/client.h>

#include <blosc2.h>


class InSitu
{
    std::string simID;  // random hash for the sim in case many sims connect to it
    int myRank, numRanks, numThreads;

    std::string jsonFilename;


    int numDatabases;
    std::vector<std::string> db_addresses;
    std::vector<yk_database_handle_t> db_handles;

    // yk_database_handle_t db_handle;

    std::map<int,int> tsNum;

    int loadDatabases();
    int putValue(int dbIndex, std::string key, std::string value);               // metadata
    int putData(int dbIndex, std::string key, char * value, size_t val_len);     // actual simlation data

    char * compress(float * data, size_t numElements, size_t & compressedSize);

    yk_database_handle_t initDB(std::string protocol, std::string serverAddr, int providerId);

  public:
    InSitu(int rank, int worldSize, std::string inputJsonFile);
    ~InSitu(){};

    int tsDone(int ts);

    void sendInfo(int myRank, int ts, std::string key, std::string value);
    void sendData(int myRank, int ts, std::string name, std::string type, std::string dataType, size_t numElements, float * data);
};




