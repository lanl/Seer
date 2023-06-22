#pragma once
#include <memory>
#include <vector>
#include <map>

struct SimData
{
    // Structure for the simulation data

    int ts;
    int processID;
    int numProcesses;
    int numRecords;
    size_t dataSize;
    char *data;   

    SimData(){ data = NULL; }
    ~SimData(){ dealloc();  }

    void dealloc(){
        if (data != NULL){
            delete []data;
            data = NULL;
        }
    }

    void copyData(char *input, size_t dataSize){
        data = new char[dataSize];
        std::memcpy(data, input, dataSize)
    }
};



class DataStore
{
    // Organize the simulation data

    std::map<int, std::vector<SimData>> tsSim;  // timestep, vector of data for each timestep
    std::map<int, int> tsNumRecv;               // timestep, number of processes it has received data for
    int numProcesses;                           // total number of processes

  public:
    DataStore();
    ~DataStore();


    int setNumProcesses(int _numProcesses){ 
        numProcesses = _numProcesses; 
    }


    bool allDataReceived(int ts)
    {
        //
        // Check if all the data has been received for a timestep

        if (tsNumRecv[ts] == numProcesses)
            return true;

        return false;
    }


    void initTs(int ts)
    {
        //
        // initialize a timestep

        std::vector<SimData> temp(numProcesses);
        tsSim[ts] = temp;
        tsNumRecv[ts] = 0;
    }


    int cleanup(int ts)
    {
        // 
        // Cleanup data

        if (tsSim.count(ts) != 0){
            // release memory - maybe an overkill
            for (int p=0; p<numProcesses; p++)
                tsSim[ts].value[p].dealloc();

            // ts
            tsSim.erase(ts);

            return 1;
        }

        return 0;
    }


    void addData(char *data, size_t dataSize)
    {
        //
        // Process what happens when new data is received

        size_t offset = 0

        // Extract some basic metadata
        int processID, numProcesses, timestep, numRecords;
        memcpy(&processID,    data + offset, sizeof(int)); offset += sizeof(int); // myRank    
        memcpy(&numProcesses, data + offset, sizeof(int)); offset += sizeof(int); // numRanks
        memcpy(&timestep,     data + offset, sizeof(int)); offset += sizeof(int); // timestep
        memcpy(&numRecords,   data + offset, sizeof(int)); offset += sizeof(int); // num records

        // First timestep
        if (tsSim.count(timestep) == 0)
            initTs(timestep);

        // Store the data
        SimData temp;
        temp.ts = timestep;
        temp.processID = processID;
        temp.numProcesses = numProcesses;
        temp.numRecords = numRecords;
        temp.copyData( data+offset, dataSize-offset) );

        tsSim[timestep].value[processID] = temp;


        // Increment the count of number of processes available for that timestep
        tsNumRecv[ts]++;
    }


    int saveToDisk(int ts)
    {
        // TODO: save the dato to disk and keep a handler for retreival

        return 1;
    }


    int retreiveFromDisk(int ts)
    {
        // TODO: save the dato to disk and keep a handler for retreival
    }
};