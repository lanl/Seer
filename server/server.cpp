#include <iostream>
#include <memory>
#include <filesystem>
#include <string>
#include <fstream>

#include <thallium.hpp>
#include <thallium/serialization/stl/string.hpp>
#include "json.hpp"

namespace tl = thallium;
std::unique_ptr<tl::engine> myEngine;


void terminateServer()
{
    myEngine->finalize();
}


// Format of msg 1
//      length of msg
//      rank
//      num_ranks
//      ts


void fn(const tl::request& req, tl::bulk& remote_bulk)
{
    tl::endpoint ep = req.get_endpoint();

    int headerSize = 4*sizeof(int);
    char * header = new char[headerSize];
    std::vector<std::pair<void*,std::size_t>> segments1(1);

    segments1[0].first  = header;
    segments1[0].second = headerSize;

    tl::bulk bulk_a = myEngine->expose(segments1, tl::bulk_mode::write_only);
    remote_bulk.on(ep).select(0, headerSize) >> bulk_a;


    int lenMsg, myRank, numRanks, ts;
    std::memcpy(&lenMsg,    header,                 sizeof(int));
    std::memcpy(&myRank,    header + sizeof(int),   sizeof(int));
    std::memcpy(&numRanks,  header + 2*sizeof(int), sizeof(int));
    std::memcpy(&ts,        header + 3*sizeof(int), sizeof(int));
    
    std::cout << "Len msg: " << lenMsg << std::endl;
    std::cout << "myRank: " << myRank << std::endl;
    std::cout << "numRanks: " << myRank << std::endl;
    std::cout << "ts: " << ts << std::endl;



    std::vector<std::pair<void*,std::size_t>> segments2(2);

    std::vector<char> w(lenMsg);

    segments2[0].first  = (void*)(&w[0]);
    segments2[0].second = w.size();

    tl::bulk bulk_b = myEngine->expose(segments2, tl::bulk_mode::write_only);
    remote_bulk.on(ep).select(headerSize, lenMsg) >> bulk_b;



    ////////////////
    // Output

    std::cout << "Server received bulk: ";

    for(auto c : w) 
        std::cout << c;
    std::cout << std::endl;

    /////////////////
        

    req.respond();
}


inline bool fileExisits(char *filename) 
{
	std::ifstream ifs(filename);
	return ifs.good();
}


inline int validateInput(int argc, char *argv[])
{
	// Check if we have the right number of arguments
	if (argc < 2)
	{
		std::cerr << "Input argument needed. Run as: <path to input JSON file>" << std::endl;
		std::cerr << "Read arguments: " << argc << std::endl;

		return 0;
	}

	// Check if input file provided exists
	if (!fileExisits(argv[1]))
	{
		std::cerr << "Could not find input JSON file " << argv[1] << "." << std::endl;
		return 0;
	}



	// Validate JSON file
	nlohmann::json jsonInput;
	std::ifstream jsonFile(argv[1]);

	try
	{
		jsonFile >> jsonInput;
	}
	catch (nlohmann::json::parse_error &e)
	{
		std::cerr << "Input JSON file " << argv[1] << " is invalid!\n"
				<< e.what() << "\n"
			    << "Validate your JSON file using e.g. https://jsonformatter.curiousconcept.com/ " << std::endl;
		return 0;
	}

    return 1;
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
    myEngine = std::make_unique<tl::engine>(serverAddress, THALLIUM_SERVER_MODE);  // infiniband address
    std::cout << "Server running at address " << myEngine->self() << std::endl;
    myEngine->define("do_rdma",fn);
}


