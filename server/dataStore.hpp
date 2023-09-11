#pragma once

#include <fstream>
#include <string>
#include <cstring>
#include <map>
#include <iostream>
#include <filesystem>
#include <unistd.h>
#include <sys/sysconf.h>


struct Info
{
    enum dataStatus{inMemory, onDisk};

    size_t dataSize;    // number of elements
    bool status;        // 0: in momory, 1: in file
};


class DataStore
{
    std::map<std::string, Info> metaData;            // key, meta-data
    std::map<std::string, char *> inMemStorage;      // key, serialized data

    size_t getAvailableMemory();    

    int writeToDisk(std::string key, char * data, size_t dataSize); // write file to disk
    char * retreiveFromDisk(std::string key);                       // read data from disk     
    
  public:
    DataStore();
    ~DataStore();

    char * retreiveData(std::string key);                           // retreive data from memory or disk
    int storeData(std::string key, char * data, size_t dataSize);   // store data to disk
};


inline DataStore::DataStore(){}


inline DataStore::~DataStore()
{
    // Clean up memory
    if (inMemStorage.size() != 0)
        for (auto& element : inMemStorage){
            delete [] element.second;
            element.second = nullptr;
        }

    // Erase from disk
    for (auto it=metaData.begin(); it!=metaData.end(); ++it)
        if (it->second.status == Info::onDisk)
            std::filesystem::remove( it->first + "file" );

    // Emptying the maps
    metaData.clear();
    inMemStorage.clear();
}


inline char * DataStore::retreiveData(std::string key)
{
    if (metaData.count(key) > 0)    // check if that data actually exists
    {
        if (metaData[key].status == Info::inMemory) // check if it's in memory
        {
            // retreive data
            char * buffer = new char[metaData[key].dataSize];
            std::memcpy(buffer, inMemStorage[key], metaData[key].dataSize);

            // cleanup
            delete [] inMemStorage[key];
            inMemStorage.erase(key);
            metaData.erase(key);

            return buffer;
        }
        else    // Info::onDisk
        {
            // retreive
            char * buffer = retreiveFromDisk(key);

            // cleanup
            metaData.erase(key);

            return buffer;
        }
    }
    else
        return nullptr;
}


inline int DataStore::storeData(std::string key, char * data, size_t dataSize)
{
    // Check how much memory we have
    bool memoryLimited = false;
    size_t availableMemory = getAvailableMemory();
    if (dataSize > availableMemory)
        memoryLimited = true;

    Info rcvInfo = {dataSize, Info::inMemory};
    if (memoryLimited)   // save to disk
    {
        writeToDisk(key, data, dataSize);

        rcvInfo.status = Info::onDisk;
        metaData[key] = rcvInfo;
    }
    else    // save in memory
    {
        char * buffer = new char[dataSize];
        std::memcpy(buffer, data, dataSize);

        inMemStorage[key] = buffer;
        metaData[key] = rcvInfo;
    }
   
    // clean up that data
    delete []data;
    data = nullptr;
}


inline size_t DataStore::getAvailableMemory()
{
    long pages = sysconf(_SC_AVPHYS_PAGES);
    long page_size = sysconf(_SC_PAGE_SIZE);
    long available_memory = pages * page_size;
    return (size_t) available_memory;
}


inline int DataStore::writeToDisk(std::string key, char * data, size_t dataSize)
{
    std::ofstream file(key + ".file");
    file.write(data, dataSize);
    file.close();

    return 1;
}


inline char * DataStore::retreiveFromDisk(std::string key)
{
    std::ifstream myfile(key + ".file");

    if (!myfile.is_open()) 
    {
        std::cout << "Error: File could not be retreive data" << std::endl;
        return nullptr;
    }

    // Get the file size
    myfile.seekg(0, std::ios::end);
    size_t fileSize = myfile.tellg();
    char *buffer = new char[fileSize];

    // Get the data
    myfile.seekg(0);
    myfile.read(buffer, fileSize);
    myfile.close();

    return buffer;
}