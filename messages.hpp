#pragma Once

#include <memory>

struct Header
{
    int type;
    uint64_t msgSize;   // in bytes

    void setData(char *msg){
        std::memcpy(&type,    msg,                  sizeof(int));        // type 
        std::memcpy(&msgSize,  msg + sizeof(int),   sizeof(uint64_t));   // size of data in msg pt 2 in bytes
    }
};


struct simDataHeader
{
    int myRank;
    int numRanks;
    int timestep;
    int numRecords;

    int setData(char *msg){
        int bufferOffset = 0;
        std::memcpy(&myRank,     msg + bufferOffset, sizeof(int)); bufferOffset += sizeof(int);  
        std::memcpy(&numRanks,   msg + bufferOffset, sizeof(int)); bufferOffset += sizeof(int); 
        std::memcpy(&timestep,   msg + bufferOffset, sizeof(int)); bufferOffset += sizeof(int);    
        std::memcpy(&numRecords, msg + bufferOffset, sizeof(int)); bufferOffset += sizeof(int); 

        return bufferOffset;
    }
};