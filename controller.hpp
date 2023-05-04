#pragma once

#include <vector>
#include <map>
#include <string>
#include <unordered_map>

#include "utility.hpp"


class Controller
{
    int numClients;
    std::map< int, std::vector<size_t> > timestepList;  // timestep, list of clients hashes
    std::unordered_map< size_t, RecvData > data;        // hash, value
    
    size_t createHash(int timestep, int id);
    int clean(int ts);

  public:
    Controller(int _numClients){ numClients = _numClients; };
    ~Controller(){};

    int newMsg(int timestep, int id, RecvData rcvData);
};


inline size_t Controller::createHash(int timestep, int id)
{
    std::string key = std::to_string(timestep) + "-" + std::to_string(id);

    std::hash<std::string> hashFn;
    std::size_t strHash = hashFn(key);
    return strHash;
}


inline int Controller::clean(int ts)
{
    for (int i=0; i<numClients; i++)
    {
        size_t hashKey = timestepList[ts][i];
        data[hashKey].clean();
    }
}


inline int Controller::newMsg(int timestep, int id, RecvData rcvData)
{
    std::map<int,std::vector<size_t>>::iterator it;

    it = timestepList.find(timestep);
    if (it != timestepList.end())
    {
        // timestep already exists

        // Create hash
        size_t hashKey = createHash(timestep, id);  

        // Add the data
        timestepList[timestep][id] = hashKey;
        data[hashKey] = rcvData;
        timestepList[timestep][numClients]++;


        if (timestepList[timestep][numClients] == numClients)   // is the number of clients at max
        {
            // start insitu process
    

            // Clean up
            clean(timestep);
        }
        else
        {
            // Do nothing
        }
    }
    else
    {
        // timestep does not exist

        // Create hash
        size_t hashKey = createHash(timestep, id);  

        // Create vector of clients
        std::vector<size_t> tempHash(numClients+1, 0);    // initilize with 0
        tempHash[id] = hashKey;
        tempHash[numClients] = 1;   // count of numbers is the last value

        // add a new entry in the list of hashes
        timestepList[timestep] = tempHash;
        data[hashKey] = rcvData;
    }
}



