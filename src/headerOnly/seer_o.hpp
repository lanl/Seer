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
#include <yokan/cxx/exception.hpp>
#include <yokan/cxx/database.hpp>

#include <blosc2.h>
#include <SZ3/api/sz.hpp>
#include <nlohmann/json.hpp>

#include "timer.hpp"
#include "utils.hpp"

#include <ifaddrs.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <net/if.h>



inline int output_ip(int world_rank, int world_size) {
    struct ifaddrs *ifaddr = nullptr, *ifa = nullptr;

    if (getifaddrs(&ifaddr) == -1) {
        perror("getifaddrs");
        return 1;
    }

    for (ifa = ifaddr; ifa != nullptr; ifa = ifa->ifa_next) {
        if (!ifa->ifa_addr)
            continue;

        // Check for IPv4 addresses
        if (ifa->ifa_addr->sa_family == AF_INET) {
            void *addr_ptr = &((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
            char addr_buffer[INET_ADDRSTRLEN] = {0};

            if (inet_ntop(AF_INET, addr_ptr, addr_buffer, INET_ADDRSTRLEN) == nullptr) {
                perror("inet_ntop");
                continue;
            }

            // Skip loopback and down interfaces
            if (!(ifa->ifa_flags & IFF_LOOPBACK) && (ifa->ifa_flags & IFF_UP)) {
                std::cout << "Rank " << world_rank << " of " << world_size
                          << " | Interface: " << ifa->ifa_name
                          << " | IPv4 Address: " << addr_buffer << std::endl;
            }
        }
    }

    freeifaddrs(ifaddr);  // Always free the allocated memory
    return 0;
}


class InSitu
{
    std::string simID;  // random hash for the sim in case many sims connect to it
    int myRank, numRanks, numThreads;

    std::string jsonFilename;
    margo_instance_id mid;

    int numDatabases;
    std::vector<std::string> db_addresses;
    std::vector<yk_database_handle_t> db_handles;

    //std::stringstream debugLog;
    std::string log;


    std::map<int,int> tsNum;

    int loadDatabases();
    int putValue(int dbIndex, std::string key, std::string value);               // metadata
    int putData(int dbIndex, std::string key, char * value, size_t val_len);     // actual simlation data

    std::string getYokanValue(int index, std::string key);
    char* getYokanData(int dbIndex, std::string key);
    
    char * compressSZ3(float * data, int x_dim, int y_dim, int z_dim, size_t & csize, std::string mode, float bound);
    char * compressBLOSC(float * data, size_t numElements, size_t & csize);

    yk_database_handle_t initDB(std::string protocol, std::string serverAddr, int providerId);

  public:
    InSitu(){};
    InSitu(int rank, int worldSize, std::string inputJsonFile="");
    ~InSitu(){};

    int tsDone(int ts);
    int simDone();
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

    output_ip(myRank, worldSize);

    std::ifstream jsonFile(inputJsonFile);
    nlohmann::json jsonInput;
    jsonFile >> jsonInput;
    simID = jsonInput["sim-id"];
    
    loadDatabases();

    if (numDatabases == 0)
        std::cout << "Error; in situ is not going to work!!!" << std::endl;


    std::string key = "_" + simID + "/num_ranks";
    putValue(0, key, std::to_string(numRanks));
}


inline int InSitu::loadDatabases()
{
    std::stringstream debugLog;
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
                debugLog << "database found at: " << serverAddr << std::endl;
            }
        }
        std::cout << myRank << " - num Databases: " << numDatabases << std::endl;
    }

    debugLog << "!!! num Databases: " << numDatabases << std::endl;
    log += debugLog.str();

    return 1;
}


inline yk_database_handle_t InSitu::initDB(std::string protocol, std::string serverAddr, int providerId)
{
    mid = margo_init(protocol.c_str(), MARGO_CLIENT_MODE, 0, 0);
    assert(mid);

    hg_addr_t server_addr = HG_ADDR_NULL;
    std::string svr_addr_str = protocol + "://" + serverAddr;
    std::cout << myRank << " ~ svr_addr_str: " << svr_addr_str << ", provider id:" <<  providerId << std::endl;

    hg_return_t hret = margo_addr_lookup(mid, svr_addr_str.c_str(), &server_addr);
    assert(hret == HG_SUCCESS);
    std::cout << myRank << " ~ margo_addr_lookup good" << std::endl;

    yk_return_t ret;
    yk_client_t client = YOKAN_CLIENT_NULL;
    ret = yk_client_init(mid, &client);
    assert(ret == YOKAN_SUCCESS);
    std::cout << myRank << " ~ yk_client_init good" << std::endl;

    yk_database_handle_t db_handle = YOKAN_DATABASE_HANDLE_NULL;
    ret = yk_database_handle_create(client, server_addr, providerId, true, &db_handle);
    assert(ret == YOKAN_SUCCESS);
    std::cout << myRank << " - All correct in initDB" << std::endl;

    return db_handle;
}





inline char * InSitu::compressBLOSC(float * data, size_t numElements, size_t & csize)
{
    std::stringstream debugLog;
    Timer clock;
    clock.start("blosc-compress");

    blosc2_init();

    size_t dataSize = sizeof(data[0]) * numElements;
    size_t osize = dataSize + BLOSC2_MAX_OVERHEAD;

    char * output = (char *) malloc(osize);
	csize = blosc1_compress(9, BLOSC_BITSHUFFLE, sizeof(data[0]), dataSize, &data[0], output, osize);
    
    debugLog << "BLOSC | Data size: " << dataSize << ", compressed size: " << csize << ", ratio: " << ((float) dataSize)/csize << std::endl;

    if (csize < 0)
		throw std::runtime_error("Compression error. Error code: " + std::to_string(osize));

	if (csize > 0)
        output = (char *) realloc(output, csize);

    blosc2_destroy();

    clock.stop("blosc-compress");
    debugLog << "Blosc compression for " << numElements << " took: " << clock.getDuration("blosc-compress") << " s." << std::endl;
    log += debugLog.str();

    return output;
}


inline char * InSitu::compressSZ3(float * data, int x_dim, int y_dim, int z_dim, size_t & csize, std::string mode, float bound)
{
    std::stringstream debugLog;

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

    char *cmpData = SZ_compress(conf, data, csize);;

    size_t dataSize = sizeof(data[0]) * x_dim * y_dim * z_dim;
    debugLog << "SZ | Original Data size: " << dataSize << ", compressed size: " << csize << ", ratio: " << ((float) dataSize)/csize << std::endl;

    if (csize < 0)
		throw std::runtime_error("Compression error.");

    clock.stop("sz3-compress");
    debugLog << "SZ3 compression for " << (x_dim*y_dim*z_dim) << " took: " << clock.getDuration("sz3-compress") << " s." << std::endl;

    log += debugLog.str();

    return cmpData;
}


inline int InSitu::putValue(int dbIndex, std::string key, std::string value)
{
    // Storing metadata
    std::cout << "put value: "<< dbIndex << ", " << key << ", " << value << std::endl;
    yk_return_t ret = yk_put(db_handles[dbIndex], YOKAN_MODE_DEFAULT, key.c_str(), key.length(), value.c_str(), value.length());

    if (ret == YOKAN_SUCCESS)
    {
        std::cout << "put value done!" << std::endl;
        return 1;
    }

    
    
    return 0;
}


inline int InSitu::putData(int dbIndex, std::string key, char * value, size_t val_len)
{
    std::stringstream debugLog;

    Timer clock;
    clock.start("put-data");

    // Specific to data that we will store compressed in the database
    yk_return_t ret = yk_put(db_handles[dbIndex], YOKAN_MODE_DEFAULT, key.c_str(), key.length(), value, val_len);

    clock.stop("put-data");
    debugLog << "Sending data to Mochi took " << clock.getDuration("put-data") << " s." << std::endl;

    log += debugLog.str();

    if (ret == YOKAN_SUCCESS)
        return 1;
    
    return 0;
}


inline std::string InSitu::getYokanValue(int index, std::string key)
{
    char *_temp = getYokanData(0, key);
    std::string temp(_temp);
    delete []_temp;

    return temp;
}


inline char* InSitu::getYokanData(int dbIndex, std::string key)
{
    Timer clock;
    std::stringstream debugLog;
    clock.start("put-data");

    std::cout << "\ngetYokanData key: " << key << ", dbindex: " << dbIndex << std::endl;

    // getting the length of the value associated with the key
    yk_return_t ret;
    size_t vsize;
    ret = yk_length(db_handles[dbIndex], YOKAN_MODE_DEFAULT, key.data(), key.length(), &vsize);
    std::cout << "vsize: " << vsize << std::endl;

    // getting the value associated with a key
    char * value_out = (char*)malloc(vsize);
    size_t value_out_size = vsize;
    ret = yk_get(db_handles[dbIndex], YOKAN_MODE_DEFAULT, key.data(), key.length(), value_out, &value_out_size);
    
    clock.stop("put-data");
    debugLog << "Sending data to Mochi took " << clock.getDuration("put-data") << " s." << std::endl;
    log += debugLog.str();
    
    return value_out;
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
    std::stringstream debugLog;

    // Check how to compress
    std::ifstream jsonFile(jsonFilename);
    nlohmann::json jsonInput;
    jsonFile >> jsonInput;

    bool bloscCompress = true;
    float bound = 750.0;
    std::string mode = "psnr";
    for (int i=0; i < jsonInput["data"].size(); i++)
    {
        if ( jsonInput["data"][i]["name"] == name)
        {
            if (jsonInput["data"][i]["compressor"] == "SZ3")
            {
                bloscCompress = false;
                mode = jsonInput["data"][i]["mode"];
                bound = jsonInput["data"][i]["value"];
            }
        }
    }


    // Send the data
    std::string key_prefix = "_" + simID + "/" + std::to_string(ts) + "/" + std::to_string(myRank) + "/" +  name;

    std::string key, value;
    yk_return_t ret;

    int dbIndex = ts%numDatabases;

    std::cout << "\n----\nsendData ~ name: " << name << ", ts: " << ts << ", numElements: " << numElements << std::endl;
    debugLog<< "\n----\nsendData ~ name: " << name << ", ts: " << ts << ", numElements: " << numElements << std::endl;

    key = key_prefix + "/num_elems";
    value = std::to_string(numElements);
    putValue(0, key, value);

    key = key_prefix + "/type";
    putValue(0, key, dataType);

    
    size_t compressedSize;
    char* compressedData;
    if (bloscCompress)
        compressedData = compressBLOSC(data, numElements, compressedSize);
    else
        compressedData = compressSZ3(data, numElements,1,1, compressedSize, mode, bound);

    key = key_prefix + "/dbIndex";
    value = std::to_string(dbIndex);
    putValue(0, key, value);

    key = key_prefix + "/compressed_size";
    value = std::to_string(compressedSize);
    putValue(0, key, value);


    key = key_prefix + "/value";
    putData(dbIndex, key, compressedData, compressedSize);      // Alternate placement of 

    

    log += debugLog.str();
    //writeLog( ("seer_" + simID + "_" + std::to_string(myRank)), log);
    appendLog( ("seer_" + simID + "_" + std::to_string(myRank)), log);
}


inline int InSitu::tsDone(int ts)
{
    std::stringstream debugLog;

    std::string key = "_" + simID + "/" + std::to_string(ts) + "/" + std::to_string(myRank) + "/status";
    putValue(0, key, "done");

    log += debugLog.str();
    //writeLog( ("seer_" + simID + "_" + std::to_string(myRank)),  log);
    appendLog( ("seer_" + simID + "_" + std::to_string(myRank)), log);

    return 1;
}


inline int InSitu::simDone()
{
    std::stringstream debugLog;

    std::string key = "_" + simID + "/" + std::to_string(myRank) + "/simDone";
    putValue(0, key, "done");

    log += debugLog.str();
    //writeLog( ("seer_" + simID + "_" + std::to_string(myRank)),  log);
    appendLog( ("seer_" + simID + "_" + std::to_string(myRank)), log);

    return 1;
}


// Data:

// _SIMID/ts/rank/name/num_elems: <value>
// _ts/rank/name/type: <value>
// _ts/rank/name/value: <compressed_data>
// _ts/rank/name/compressed_size: <value>

