#include <mpi.h>
#include <stdio.h>
#include <cstring>
#include <iostream>
#include <memory>
#include <filesystem>
#include <string>
#include <sstream>

#include <thallium.hpp>
#include <thallium/serialization/stl/string.hpp>

#inlcude "dataStore.hpp"

std::unique_ptr<thallium::engine> myEngine;
DataStore db;


inline void processData(char *recvData, size_t recvDataSize)
{
    size_t index = 0;

    // read the first byte to determine the kind of message this is
    std::unique_ptr<char[]> buffer(new char[8]);
    std::memcpy(buffer, recvData+index, 8);   index += 8;
    std::string header(buffer);


    if (header == "01") // metadata
    {

    } 
    else if (header == "02") // simulation data
    {
        std::string buff;

        // Get the metadata
        std::string simId;
        int timestep, dataRank;
        
        std::memcpy(buffer, recvData+index, 8); index += 8;
        simId.assign(buffer);

        std::memcpy(buffer, recvData+index, 8); index += 8;
        buff.assign(buffer);
        dataRank = stoi(buffer);

        std::memcpy(buffer, recvData+index, 8); index += 8;
        buff.assign(buffer);
        timestep = stoi(buffer);

        // Get the data
        size_t dataSize = recvDataSize - index;
        char* dataBuffer = new char[dataSize];
        std::memcpy(dataBuffer, recvData+index, dataSize);


        // Store the data
        std::string key = simId + "_" + dataRank.c_str() + "_" + timestep.c_str();
        db.storeData(key, dataBuffer, dataSize);
    }
}


inline void startServer(std::string serverAddress)
{
    myEngine = std::make_unique<thallium::engine>(serverAddress, THALLIUM_SERVER_MODE);  // infiniband address
    std::cout << "Server running at address " << myEngine->self() << std::endl;
    myEngine->define("do_rdma",recvFn);
}


inline void terminateServer()
{
    myEngine->finalize();
}


inline void recv(const thallium::request& req, thallium::bulk& remote_bulk)
{
    thallium::endpoint ep = req.get_endpoint();

    //
    // find the size of the message

    // Create the buffer
    int headerSize = sizeof(uint64_t);    // size of message
    char *header = new char[headerSize];

    // Get the header
    std::vector<std::pair<void*,std::size_t>> segments1(1);     // message part 1, the header
    segments1[0].first  = header;
    segments1[0].second = headerSize;

    thallium::bulk bulk_a = myEngine->expose(segments1, thallium::bulk_mode::write_only);
    remote_bulk.on(ep).select(0, headerSize) >> bulk_a;

    int lenMsg;
    std::memcpy(&lenMsg,  header,   sizeof(uint64_t));   // size of data in msg pt 2 in bytes
    delete []header;



    //
    // Get the actual data
    std::vector<std::pair<void*,std::size_t>> segments2(1);     // message part 2, the data

    // Create buffer
    if (revData != nullptr)
    {
        delete []rcvData;
        rcvData = nullptr;
    }

    char *rcvData = new char[lenMsg];

    segments2[0].first  = rcvData;
    segments2[0].second = lenMsg;

    thallium::bulk bulk_b = myEngine->expose(segments2, thallium::bulk_mode::write_only);
    remote_bulk.on(ep).select(headerSize, lenMsg) >> bulk_b;

    processData(rcvData, lenMsg);

    req.respond();
}




int main(int argc, char** argv)
{
    // Initialize MPI
    int worldSize, worldRank;
    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &worldSize);
    MPI_Comm_rank(MPI_COMM_WORLD, &worldRank);


    //
	// Validate input params
	if (!validateInput(argc, argv))
		return 0;


    // Pass JSON file to json parser
	nlohmann::json jsonInput;
	std::ifstream jsonFile(argv[1]);
	jsonFile >> jsonInput;


    // Get data server address
    std::string serverIp = jsonInput["server"]["ip"];
    int serverPort = jsonInput["server"]["port"];
    std::string serverAddress = "tcp://" + serverIp + ":" + std::to_string(serverPort);

    // start data server
    recvServer::startServer(serverAddress);



    // Get the address of the yokan server

    // start the comm server (with client)




    // Clean up
    MPI_Finalize();
}