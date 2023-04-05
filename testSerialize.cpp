#include <vector>
#include <iostream>

int main(int argc, char** argv) 
{
    int headerLength = 25;
    std::vector<char> header(headerLength, '_');

    

    

    

    char type = 'd'; // d: data, i:initilize
    //std::string 

    header.insert( header.begin(), 1, type);


    std::string s(header.begin(), header.end());
    std::cout <<  s << std::endl;

    return 0;
}