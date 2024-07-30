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

#include "timer.hpp"
#include "utils.hpp"


class SeerClient
{
    std::stringstream debugLog;

    std::string simID; 
    std::string jsonInputFile;


    int numDatabases;
    std::vector<std::string> db_addresses;
    std::vector<yk_database_handle_t> db_handles;

    yk_database_handle_t initDB(std::string protocol, std::string serverAddr, int providerId);
    int loadDatabases();

    char* getYokanData(int dbIndex, std::string key);
    std::string getYokanValue(int index, std::string key);

    int decompressBLOSC(char * cdata, float * data, size_t numElements);
    int decompressSZ3(char * cmpData, int x_dim, int y_dim, int z_dim, size_t cmpSize, float decData[], std::string mode, float bound);


  public:
    SeerClient(){};
    ~SeerClient(){};

    void init(std::string inputJsonFile);

    int getNumRanks();
    bool isTimestepReady(int ts);
    float * getData(int ts, int rank, std::string variable, size_t & numElements, std::string &metadata);
};


inline int SeerClient::loadDatabases()
{
    if (jsonInputFile != "")
    {
        std::ifstream jsonFile(jsonInputFile);
        nlohmann::json jsonInput;
        jsonFile >> jsonInput;

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
                debugLog << "database found at: " << serverAddr << std::endl;
            }
        }
    }

    debugLog << "num Databases: " << numDatabases << std::endl;
    std::cout << "num Databases: " << numDatabases << std::endl;

    return 1;
}


inline yk_database_handle_t SeerClient::initDB(std::string protocol, std::string serverAddr, int providerId)
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
    debugLog<< "initDB: " << (ret == YOKAN_SUCCESS) << std::endl;

    assert(ret == YOKAN_SUCCESS);

    return db_handle;
}



inline std::string SeerClient::getYokanValue(int index, std::string key)
{
    char *_temp = getYokanData(0, key);
    std::string temp(_temp);
    delete []_temp;

    return temp;
}


inline char* SeerClient::getYokanData(int dbIndex, std::string key)
{
    Timer clock;
    clock.start("put-data");

    debugLog << "\ngetYokanData key: " << key << ", dbindex: " << dbIndex << std::endl;

    // getting the length of the value associated with the key
    yk_return_t ret;
    size_t vsize;
    ret = yk_length(db_handles[dbIndex], YOKAN_MODE_DEFAULT, key.data(), key.length(), &vsize);
    debugLog << "vsize: " << vsize << std::endl;

    // getting the value associated with a key
    char * value_out = (char*)malloc(vsize);
    size_t value_out_size = vsize;
    ret = yk_get(db_handles[dbIndex], YOKAN_MODE_DEFAULT, key.data(), key.length(), value_out, &value_out_size);
    
    clock.stop("put-data");
    debugLog << "Sending data to Mochi took " << clock.getDuration("put-data") << " s." << std::endl;

    return value_out;
}


inline int SeerClient::decompressBLOSC(char * cdata, float *data, size_t numElements)
{
    Timer clock;
    clock.start("blosc-decompress");

    blosc2_init();

    size_t dataSize = (sizeof(float) * numElements);// + BLOSC2_MAX_OVERHEAD;
    size_t decsize = blosc1_decompress(cdata, data, dataSize);


    debugLog << "dataSize: " << dataSize << ", decsize: " << decsize << std::endl;
    
    if (decsize < 0)
		throw std::runtime_error("Decompression error. Error code: " + std::to_string(decsize));

    blosc2_destroy();

    clock.stop("blosc-decompress");
    debugLog << "Blosc | decompressed size: " << decsize  << ", took: " << clock.getDuration("blosc-decompress") << " s." << std::endl;

    return decsize;
}


inline int SeerClient::decompressSZ3(char * cmpData, int x_dim, int y_dim, int z_dim, size_t cmpSize, float *decData, std::string mode, float bound)
{
    Timer clock;
    clock.start("sz3-compress");

    SZ3::Config conf({x_dim, y_dim, z_dim});
    conf.cmprAlgo = SZ3::ALGO_INTERP_LORENZO;
    if (mode == "psnr"){
        conf.errorBoundMode = SZ3::EB_PSNR; 
        conf.psnrErrorBound = bound; 
    } else if (mode == "abs"){
        conf.errorBoundMode = SZ3::EB_ABS; 
        conf.absErrorBound = bound; 
    } else if (mode == "rel"){
        conf.errorBoundMode = SZ3::EB_REL; 
        conf.relErrorBound = bound; 
    }

    SZ_decompress(conf, cmpData, cmpSize, decData);


    clock.stop("sz3-compress");
    debugLog << "SZ3 compression for " << (x_dim*y_dim*z_dim) << " took: " << clock.getDuration("sz3-compress") << " s." << std::endl;

    return 1;
}



inline void SeerClient::init(std::string inputJsonFile)
{
    std::ifstream jsonFile(inputJsonFile);
    nlohmann::json jsonInput;
    jsonFile >> jsonInput;
    numDatabases = 0;

    simID = jsonInput["sim-id"];
    jsonInputFile = inputJsonFile;
    
    loadDatabases();

    debugLog << "simID: " << simID << ", jsonInputFile: " << jsonInputFile << std::endl;

    if (numDatabases == 0)
        std::cout << "Error; in situ is not going to work!!!" << std::endl;
}




inline bool SeerClient::isTimestepReady(int ts)
{
    std::string key, value;
    key = "_" + simID + "/num_ranks";
    debugLog << "isTimestepReady, key: " << key << std::endl;
    value = getYokanValue(0, key);

    int numRanks = std::stoi(value);


    int count = 0;
    for (int r=0; r<numRanks; r++)
    {
        key = "_" + simID + "/" + std::to_string(ts) + "/" + std::to_string(r) + "/status";
        value = getYokanValue(0, key);
        if (value == "done")
            count++;
    }

    if (count == numRanks)
        return true;
    else
        return false;
}


inline int SeerClient::getNumRanks()
{
    std::string key = "_" + simID + "/num_ranks";
    std::string numRanks = getYokanValue(0, key);

    return std::stoi(numRanks);
}


inline float * SeerClient::getData(int ts, int rank, std::string variable, size_t & numElements, std::string &metadata)
{
    std::string key;
    std::string key_prefix = "_" + simID + "/" + std::to_string(ts) + "/" + std::to_string(rank) + "/" +  variable + "/";

    numElements = std::stoi( getYokanValue(0, (key_prefix + "num_elems")) );
    int compressedSize = std::stoi( getYokanValue(0, (key_prefix + "compressed_size")) );
    std::string dataType  = getYokanValue(0, (key_prefix + "type"));
    int dbIndex = std::stoi( getYokanValue(0, (key_prefix + "dbIndex")) );


    debugLog << "\n num_elems: " << numElements << std::endl;
    debugLog << "variable: " << variable << std::endl;
    debugLog << "compressed_size: " << compressedSize << std::endl;
    debugLog << "type: " << dataType << std::endl;
    debugLog << "dbIndex: " << dbIndex << std::endl;

    metadata = "type: " + dataType + ", num_elems:" + std::to_string(numElements);
    

    // Compression
    std::ifstream jsonFile(jsonInputFile);
    nlohmann::json jsonInput;
    jsonFile >> jsonInput;

    bool bloscCompress = true;
    float bound;
    std::string mode = "psnr";
    for (int i=0; i < jsonInput["data"].size(); i++)
    {
        if ( jsonInput["data"][i]["name"] == variable)
        {
            if (jsonInput["data"][i]["compressor"] == "SZ3")
            {
                bloscCompress = false;
                mode = jsonInput["data"][i]["mode"];
                bound = jsonInput["data"][i]["value"];
            }
        }
    }
    
    debugLog << "mode: " << mode << std::endl;
    debugLog << "bound: " << bound << std::endl;
    debugLog << "bloscCompress: " << bloscCompress << std::endl;

    char * cdata = getYokanData(dbIndex, (key_prefix + "value") );
    float *data = new float[numElements];

    if (bloscCompress)
        decompressBLOSC(cdata, data, numElements);
    else
        decompressSZ3(cdata, numElements, 1, 1, compressedSize, data, mode, bound);


    writeLog( ("seer_" + simID + "_" + std::to_string(rank)), debugLog.str());

    return &data[0];
}