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

#include <yokan/client.h>
#include <yokan/database.h>

#include <blosc2.h>
#include <SZ3/api/sz.hpp>
#include <nlohmann/json.hpp>


class InSitu
{
    std::string simID;  // random hash for the sim in case many sims connect to it
    int myRank, numRanks, numThreads;

    std::string jsonFilename;

    int numDatabases;
    std::vector<std::string> db_addresses;
    std::vector<yk_database_handle_t> db_handles;


    std::map<int,int> tsNum;

    int loadDatabases();
    int putValue(int dbIndex, std::string key, std::string value);               // metadata
    int putData(int dbIndex, std::string key, char * value, size_t val_len);     // actual simlation data

    char * compressSZ3(float * data, int x_dim, int y_dim, int z_dim, size_t & csize, float bound);
    char * compressBLOSC(float * data, size_t numElements, size_t & csize);

    yk_database_handle_t initDB(std::string protocol, std::string serverAddr, int providerId);

  public:
    InSitu(){};
    InSitu(int rank, int worldSize, std::string inputJsonFile="");
    ~InSitu(){};

    int tsDone(int ts);
    void init(int rank, int worldSize, std::string inputJsonFile="");

    void sendInfo(int myRank, int ts, std::string key, std::string value);
    void sendData(int myRank, int ts, std::string name, std::string type, std::string dataType, size_t numElements, float * data);
    void sendDataTest(int myRank, int ts, std::string name, std::string type, std::string dataType, size_t numElements);
};



inline InSitu::InSitu(int rank, int worldSize, std::string inputJsonFile)
{ 
    init(rank, worldSize, inputJsonFile);
}


inline void InSitu::init(int rank, int worldSize, std::string inputJsonFile)
{ 
    myRank = rank; 
    numRanks = worldSize; 
    numThreads = 2;
    numDatabases = 0;
    jsonFilename = inputJsonFile;

    srand(time(0));
    simID = rand();

    std::ifstream jsonFile(inputJsonFile);
    nlohmann::json jsonInput;
    jsonFile >> jsonInput;
    simID = jsonInput["sim-id"];
    
    loadDatabases();

    if (numDatabases == 0)
        std::cout << "Error; in situ is not going to work!!!" << std::endl;
}

inline int InSitu::loadDatabases()
{
    if (jsonFilename != "")
    {
        std::ifstream jsonFile(jsonFilename);
        nlohmann::json jsonInput;
        jsonFile >> jsonInput;

        //numDatabases = jsonInput["databases"].size();
        for (int i=0; i<jsonInput["databases"].size(); i++)
        {
            std::string serverAddr = jsonInput["databases"][i]["address"];

            if ( std::find(db_addresses.begin(), db_addresses.end(), serverAddr) == db_addresses.end() ) // not found
            {
                std::string protocol = jsonInput["databases"][i]["protocol"];
                int providerId = jsonInput["databases"][i]["provider_id"];

                db_handles.push_back( initDB(protocol, serverAddr, providerId) );
                db_addresses.push_back( serverAddr );

                numDatabases++;
                std::cout << "database found at: " << serverAddr << std::endl;
            }
        }
        std::cout << "num Databases: " << numDatabases << std::endl;
    }

    return 1;
}


inline yk_database_handle_t InSitu::initDB(std::string protocol, std::string serverAddr, int providerId)
{
    margo_instance_id mid = margo_init(protocol.c_str(), MARGO_CLIENT_MODE, 0, 0);
    assert(mid);

    hg_addr_t server_addr = HG_ADDR_NULL;
    std::string svr_addr_str = protocol + "://" + serverAddr;
    hg_return_t hret = margo_addr_lookup(mid, svr_addr_str.c_str(), &server_addr);
    assert(hret == HG_SUCCESS);

    yk_return_t ret;
    yk_client_t client = YOKAN_CLIENT_NULL;

    ret = yk_client_init(mid, &client);
    assert(ret == YOKAN_SUCCESS);

    yk_database_handle_t db_handle = YOKAN_DATABASE_HANDLE_NULL;
    ret = yk_database_handle_create(client, server_addr, providerId, true, &db_handle);
    assert(ret == YOKAN_SUCCESS);

    return db_handle;
}



inline int InSitu::tsDone(int ts)
{
    int dbIndex = ts%numDatabases;

    std::cout << "tsDone, dbIndex: " << dbIndex << std::endl;
    if (tsNum.find(ts) == tsNum.end()) 
        tsNum[ts] = 1;
    else
        tsNum[ts] = tsNum[ts]+1;


    std::string key = "_" + simID + "/" + std::to_string(ts) + "/status";
    if (tsNum[ts] == numRanks)
    {
        putValue(dbIndex, key, "ready");
        loadDatabases();    // periodically check for new databases
    }
    else
        putValue(dbIndex, key, std::to_string(tsNum[ts]));

    return 1;
}


inline char * InSitu::compressBLOSC(float * data, size_t numElements, size_t & csize)
{
    blosc2_init();

    size_t dataSize = sizeof(data[0]) * numElements;
    //std::cout << "dataSize: " << dataSize << std::endl;
    size_t osize = dataSize + BLOSC2_MAX_OVERHEAD;

    char * output = (char *) malloc(osize);
	csize = blosc1_compress(9, BLOSC_BITSHUFFLE, sizeof(data[0]), dataSize, &data[0], output, osize);
    
    std::cout << "BLOSC | Data size: " << dataSize << ", compressed size: " << csize << ", osize: " << osize << std::endl;

    if (csize < 0)
		throw std::runtime_error("Compression error. Error code: " + std::to_string(osize));

	if (csize > 0)
        output = (char *) realloc(output, csize);

    blosc2_destroy();
    return output;
}


inline char * InSitu::compressSZ3(float * data, int x_dim, int y_dim, int z_dim, size_t & csize, float bound)
{
    SZ3::Config conf({x_dim, y_dim, z_dim});
    conf.cmprAlgo = SZ3::ALGO_INTERP_LORENZO;
    conf.errorBoundMode = SZ3::EB_PSNR; 
    conf.psnrErrorBound = bound; 


    char *cmpData = SZ_compress(conf, data, csize);;

    size_t dataSize = sizeof(data[0]) * x_dim * y_dim * z_dim;
    std::cout << "SZ | Original Data size: " << dataSize << ", compressed size: " << csize  << std::endl;

    if (csize < 0)
		throw std::runtime_error("Compression error.");

    return cmpData;
}


inline int InSitu::putValue(int dbIndex, std::string key, std::string value)
{
    // Storing metadata
    yk_return_t ret = yk_put(db_handles[dbIndex], YOKAN_MODE_DEFAULT, key.c_str(), key.length(), value.c_str(), value.length());

    if (ret == YOKAN_SUCCESS)
        return 1;
    
    return 0;
}


inline int InSitu::putData(int dbIndex, std::string key, char * value, size_t val_len)
{
    // Specific to data that we will store compressed in the database
    yk_return_t ret = yk_put(db_handles[dbIndex], YOKAN_MODE_DEFAULT, key.c_str(), key.length(), value, val_len);

    if (ret == YOKAN_SUCCESS)
        return 1;
    
    return 0;
}



inline void InSitu::sendInfo(int myRank, int ts, std::string key, std::string value)
{
    std::string key_prefix = "_" + simID + "/" + std::to_string(ts) + "/" + std::to_string(myRank) + "/" ;
    std::string k = key_prefix + key;

    int dbIndex = ts%numDatabases;

    putValue(dbIndex, k, value);
}


inline void InSitu::sendDataTest(int myRank, int ts, std::string name, std::string type, std::string dataType, size_t numElements)
{
    std::cout << myRank << " ~ Seer, ts: " << ts << std::endl;
}



inline void InSitu::sendData(int myRank, int ts, std::string name, std::string type, std::string dataType, size_t numElements, float * data)
{
    // Check how to compress
    std::ifstream jsonFile(jsonFilename);
    nlohmann::json jsonInput;
    jsonFile >> jsonInput;

    bool bloscCompress = true;
    float psnr = 100.0;
    for (int i=0; i < jsonInput["data"].size(); i++)
    {
        if ( jsonInput["data"][i]["name"] == name)
        {
            if (jsonInput["data"][i]["compressor"] == "SZ3")
            {
                bloscCompress = false;
                psnr = jsonInput["data"][i]["psnr"];
            }
        }
    }


    // Send the data
    std::string key_prefix = "_" + simID + "/" + std::to_string(ts) + "/" + std::to_string(myRank) + "/" +  name;

    std::string key, value;
    yk_return_t ret;

    int dbIndex = ts%numDatabases;
    std::cout << "numElements: " << numElements << std::endl;

    key = key_prefix + "/num_elems";
    value = std::to_string(numElements);
    putValue(dbIndex, key, value);

    key = key_prefix + "/type";
    putValue(dbIndex, key, dataType);

    
    size_t compressedSize;
    char* compressedData;
    if (bloscCompress)
        compressedData = compressBLOSC(data, numElements, compressedSize);
    else
        compressedData = compressSZ3(data, numElements,1,1, compressedSize, psnr);

    key = key_prefix + "/value";
    std::cout << "compressed string size: " << compressedSize << std::endl;
    putData(dbIndex, key, compressedData, compressedSize);

    key = key_prefix + "/compressed_size";
    value = std::to_string(compressedSize);
    putValue(dbIndex, key, value);
}


// Data:

// _SIMID_ts/rank/name/num_elems: <value>
// _ts/rank/name/type: <value>
// _ts/rank/name/value: <compressed_data>
// _ts/rank/name/compressed_size: <value>

