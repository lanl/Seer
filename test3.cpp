#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <cstring>

#include "utility.hpp"
#include "sender.hpp"
//#include "receiver.hpp"


int main(int argc, char** argv) 
{
    // Data
    std::vector<float> x;
    x.push_back(0.0);
    x.push_back(1.0);
    x.push_back(2.0);
    x.push_back(3.0);
    x.push_back(0.0);
    x.push_back(1.0);
    x.push_back(2.0);
    x.push_back(3.0);

    std::vector<float> y;
    y.push_back(0);
    y.push_back(1);
    y.push_back(1);
    y.push_back(0);
    y.push_back(1);
    y.push_back(2);
    y.push_back(2);
    y.push_back(0);

    std::vector<float> temp1;
    temp1.push_back(210);
    temp1.push_back(-0.4);
    temp1.push_back(5);
    temp1.push_back(1000);
    temp1.push_back(3.4);
    temp1.push_back(-4.4);
    temp1.push_back(6.2);
    temp1.push_back(-0.1);


    Sender send;
    Receiver recv;


    // Specify Header
    std::map<std::string, std::string> headerX;
    headerX["name"] = "x";
    headerX["type"] = "float";
    headerX["size"] = std::to_string(x.size());
    headerX["topology"] = "vertex";
    headerX["desc"] = "x-coordinate";

    std::map<std::string, std::string> headerY;
    headerY["name"] = "y";
    headerY["type"] = "float";
    headerY["size"] = std::to_string(y.size());
    headerY["topology"] = "vertex";
    headerY["desc"] = "y-coordinate";

    std::map<std::string, std::string> headerTemp;
    headerTemp["name"] = "temperature";
    headerTemp["type"] = "float";
    headerTemp["size"] = std::to_string(temp1.size());
    headerTemp["topology"] = "vertex";
    headerTemp["desc"] = "temperature in the sim";


    // Test sending
    send.setData(headerX, &x[0], x.size());
    send.setData(headerY, &y[0], y.size());
    send.setData(headerTemp, &temp1[0], temp1.size());

    std::cout << "set data" << std::endl;

    int dataSize = 0;
    char * buffer = send.sendData(dataSize);
  
    std::cout << "sent data " << dataSize << "\n@@@@@@@@@@@@@@@@@@@@\n" << std::endl;


    // Test Receiving
    // recv.receiveData(buffer,dataSize);
    
    // std::cout << "recv data" << std::endl;

    return 0;
}