#pragma once

#include <sstream>
#include <string>
#include <map>
#include <fstream>

#include "json.hpp"



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


inline bool allocateMemory(void*& data, std::string datatype, size_t numElems) 
{
	if (datatype == "int")
		data = new int[numElems];
	else if (datatype == "float")
		data = new float[numElems];
	else if (datatype == "double")
		data = new double[numElems];
	else if (datatype == "int8_t")
		data = new int8_t[numElems];
	else if (datatype == "int16_t")
		data = new int16_t[numElems];
	else if (datatype == "int32_t")
		data = new int32_t[numElems];
	else if (datatype == "int64_t")
		data = new int64_t[numElems];
	else if (datatype == "uint8_t")
		data = new uint8_t[numElems];
	else if (datatype == "uint16_t")
		data = new uint16_t[numElems];
	else if (datatype == "uint32_t")
		data = new uint32_t[numElems];
	else if (datatype == "uint64_t")
		data = new uint64_t[numElems];
	else
		return false;

	return true;
}


inline bool release(void*& data, std::string datatype) 
{
	if (data == nullptr) // already deallocated!
		return true;

	if (datatype == "int")
		delete[](int *) data;
	else if (datatype == "float")
		delete[](float *) data;
	else if (datatype == "double")
		delete[](double *) data;
	else if (datatype == "int8_t")
		delete[](int8_t *) data;
	else if (datatype == "int16_t")
		delete[](int16_t *) data;
	else if (datatype == "int32_t")
		delete[](int32_t *) data;
	else if (datatype == "int64_t")
		delete[](int64_t *) data;
	else if (datatype == "uint8_t")
		delete[](uint8_t *) data;
	else if (datatype == "uint16_t")
		delete[](uint16_t *) data;
	else if (datatype == "uint32_t")
		delete[](uint32_t *) data;
	else if (datatype == "uint64_t")
		delete[](uint64_t *) data;
	else
		return false;

	data = nullptr;
	return true;
}


inline int getTypeSize(std::string type)
{
    if (type == "int")
        return sizeof(int);
    else if (type == "float")
        return sizeof(float);
    else if (type == "double")
        return sizeof(double);
    else
    {
        std::cout << "Error: Type " << type << " is undefined!!!" << std::endl;
        return 0;
    }
}


struct RecvData
{
	std::map<std::string, std::string> header;
    void * data;

    void clean(){
        release(data, header["type"]);
    };
};