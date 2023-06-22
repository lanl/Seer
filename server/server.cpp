#include <iostream>
#include <memory>
#include <filesystem>
#include <string>
#include <sstream>

#include <thallium.hpp>
#include <thallium/serialization/stl/string.hpp>

#include "receiver.hpp"
#include "json.hpp"
#include "utility.hpp"
#include "dataStore.hpp"

std::stringstream log;
std::unique_ptr<thallium::engine> myEngine;

DataStore dataStore;    // storing the data received from the server

inline void terminateServer()
{
    myEngine->finalize();
}


inline void processData(int msgType, char *data)
{
    if (msgType == 0)   // simulation Data
    {
        // set num timsteps
    }
    else if (msgType == 1)   // simulation Data
    {
        // transfer data
    }
    else if (msgType == 3) // data analysis
    {

    }
}

// Format of msg 1
//      nsg type
//      length


void fn(const thallium::request& req, thallium::bulk& remote_bulk)
{
    thallium::endpoint ep = req.get_endpoint();

    int headerSize = 4*sizeof(int);
    char * header = new char[headerSize];
    std::vector<std::pair<void*,std::size_t>> segments1(1);     // message part 1, the header

    segments1[0].first  = header;
    segments1[0].second = headerSize;

    thallium::bulk bulk_a = myEngine->expose(segments1, thallium::bulk_mode::write_only);
    remote_bulk.on(ep).select(0, headerSize) >> bulk_a;


    int lenMsg, myRank, numRanks, ts;
    std::memcpy(&lenMsg,    header,                 sizeof(int));
    std::memcpy(&myRank,    header + sizeof(int),   sizeof(int));
    std::memcpy(&numRanks,  header + 2*sizeof(int), sizeof(int));
    std::memcpy(&ts,        header + 3*sizeof(int), sizeof(int));
    
    std::cout << "Len msg: " << lenMsg << std::endl;
    std::cout << "myRank: " << myRank << std::endl;
    std::cout << "numRanks: " << numRanks << std::endl;
    std::cout << "ts: " << ts << std::endl;



    std::vector<std::pair<void*,std::size_t>> segments2(1);     // message part 2, the data

    //std::vector<char> w(lenMsg);
    //std::vector<char> w(lenMsg);
    char *rcvData = new char[lenMsg];

    // segments2[0].first  = (void*)(&w[0]);
    // segments2[0].second = w.size();

    segments2[0].first  = (void*)(&w[0]);
    segments2[0].second = w.size();

    thallium::bulk bulk_b = myEngine->expose(segments2, thallium::bulk_mode::write_only);
    remote_bulk.on(ep).select(headerSize, lenMsg) >> bulk_b;

    int numMsgs;
    std::memcpy(&numMsgsw,                 sizeof(int));

    ////////////////
    // Output

    std::cout << "Server received bulk: ";

    for(auto c : w) 
        std::cout << c;
    std::cout << std::endl;

    /////////////////
        

    req.respond();
}



inline void recvFn(const thallium::request& req, thallium::bulk& remote_bulk)
{
    thallium::endpoint ep = req.get_endpoint();

    //
    // Get infomration about the message

    // Create the buffer
    int headerSize = sizeof(int) + sizeof(uint64_t);
    char *header = new char[headerSize];

    // Get the header
    std::vector<std::pair<void*,std::size_t>> segments1(1);     // message part 1, the header
    segments1[0].first  = header;
    segments1[0].second = headerSize;

    thallium::bulk bulk_a = myEngine->expose(segments1, thallium::bulk_mode::write_only);
    remote_bulk.on(ep).select(0, headerSize) >> bulk_a;

    int lenMsg, type;
    std::memcpy(&type,    header,                 sizeof(int));        // type 
    std::memcpy(&lenMsg,  header + sizeof(int),   sizeof(uint64_t));   // size of data in msg pt 2 in bytes
    delete []header;
    
    // std::cout << "Len msg2: " << lenMsg << std::endl;
    // std::cout << "Msg type: " << type << std::endl;

    log << "Len msg2: " << lenMsg << std::endl;
    log << "Msg type: " << type << std::endl;



    //
    // Get the actual data
    std::vector<std::pair<void*,std::size_t>> segments2(1);     // message part 2, the data

    // Create buffer
    char *rcvData = new char[lenMsg];

    segments2[0].first  = rcvData;
    segments2[0].second = lenMsg;

    thallium::bulk bulk_b = myEngine->expose(segments2, thallium::bulk_mode::write_only);
    remote_bulk.on(ep).select(headerSize, lenMsg) >> bulk_b;

    req.respond();

    processData(type,rcvData);
}








int main(int argc, char** argv) 
{
    //
	// Validate input params
	if (!validateInput(argc, argv))
		return 0;

    // Pass JSON file to json parser
	nlohmann::json jsonInput;
	std::ifstream jsonFile(argv[1]);
	jsonFile >> jsonInput;


    // Get server address
    std::string serverIp = jsonInput["server"]["ip"];
    int serverPort = jsonInput["server"]["port"];
    std::string serverAddress = "tcp://" + serverIp + ":" + std::to_string(serverPort);




    // start server
    myEngine = std::make_unique<thallium::engine>(serverAddress, THALLIUM_SERVER_MODE);  // infiniband address
    std::cout << "Server running at address " << myEngine->self() << std::endl;
    myEngine->define("do_rdma",recvFn);
}


