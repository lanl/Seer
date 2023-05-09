#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <cstring>

#include "utility.hpp"




inline char * serializeMap(std::map<std::string, std::string> header, size_t &headerSize)
{
    std::stringstream ss;
    for (auto const& i : header)
        ss << i.first << ":" << i.second << "\n";
    
    std::string tempStr = ss.str();
    headerSize = tempStr.length()+1;

    char * tempBuf = new char[headerSize];
    strcpy(tempBuf, tempStr.c_str());

    return tempBuf;
}


inline std::map<std::string, std::string> deserializeMap(char * buff, size_t headerSize)
{
    std::string tempStr = std::string(buff, headerSize); 
    std::stringstream ss(tempStr);  

    std::cout << "deserialize map:" << tempStr << std::endl;
    std::map<std::string, std::string> header;
    std::string word;
    while (std::getline(ss, word)) { // Extract word from the stream.
        size_t found = word.find(':');
        if (found!=std::string::npos)
        {
            std::string key = word.substr(0,found); 
            header[key] = word.substr(found+1,word.length());
            std::cout <<  "header[" << key << "]: " << header[key] << std::endl;
        }
    }

    return header;
}


template <typename T>
char * serializeData(std::map<std::string, std::string> header, T* data, size_t numElements)
{
    // Header information
    size_t headerSize = 0;
    char * serializedHeader = serializeMap(header, headerSize);


    // Create Buffer
    size_t dataSize = numElements * getTypeSize(header["type"]);  //sizeof(float);

    size_t totalSize = sizeof(int) +  // offset
                        headerSize +
                        dataSize;

    char *buffer = new char[ totalSize ];


    // Copy data to buffer
    int offset = headerSize;
    memcpy( buffer,                            &offset,   sizeof(int) );
    memcpy( buffer + sizeof(int),              &serializedHeader[0], headerSize );
    memcpy( buffer + sizeof(int) + headerSize, &data[0], dataSize );


    delete []serializedHeader;

    return buffer;
}



void deserializeData(char * buffer, std::map<std::string, std::string> &header, void * &data)
{
    // Get header Size
    int headerSize;
    std::memcpy(&headerSize, buffer, sizeof(int));
    std::cout << "header size: " << headerSize << std::endl;


    // Get header back
    char * headerBuf = new char[headerSize];
    std::memcpy(headerBuf, buffer + sizeof(int), headerSize);
    header = deserializeMap(headerBuf, headerSize);


    // Get data back
    allocateMemory(data, header["type"], std::stoi(header["size"]));

    int dataSize = std::stoi(header["size"]) * getTypeSize(header["type"]);
    std::memcpy(data, buffer + (sizeof(int) + headerSize), dataSize);

    // float * output = new float[stoi(header["size"])];
    // memcpy(output, data, dataSize);
    // for (int i=0; i<stoi(header["size"]); i++)
    //     std::cout << output[i] << std::endl;

    // for (int i=0; i<stoi(header["size"]); i++)
    //      std::cout << static_cast<float *>(data)[i] << std::endl;
}


int main(int argc, char** argv) 
{
    // Data


    std::vector<float> temp1;
    temp1.push_back(210);
    temp1.push_back(-0.4);
    temp1.push_back(5);
    temp1.push_back(1000);
    temp1.push_back(3.4);
    temp1.push_back(-4.4);
    temp1.push_back(6.2);
    temp1.push_back(-0.1);

    std::map<std::string, std::string> headerTemp;
    headerTemp["name"] = "temperature";
    headerTemp["type"] = "float";
    headerTemp["size"] = std::to_string(temp1.size());
    headerTemp["topology"] = "vertex";
    headerTemp["desc"] = "just a test";

  

    // Serialize data
    char *buffer = serializeData(headerTemp, &temp1[0],  temp1.size());


    // Send Data




    // Recv Data


    //////////////////////////////////////////////////////////
    std::cout << "\n------------------------" << std::endl;

    std::map<std::string, std::string> recvHeader;


    void * recvData = NULL;
    deserializeData(buffer, recvHeader, recvData);

    // for (int i=0; i<stoi(recvHeader["size"]); i++)
    //     std::cout << static_cast<float *>(recvData)[i] << std::endl;

    float * output = new float[ stoi(recvHeader["size"]) ];
    memcpy(output, recvData, stoi(recvHeader["size"])*sizeof(float));
    for (int i=0; i<stoi(recvHeader["size"]); i++)
        std::cout << output[i] << std::endl;



    return 0;
}