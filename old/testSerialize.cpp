#include <vector>
#include <iostream>
#include <string>


struct msg
{
    char type;
    int headerSize;
    int dataSize;
};


std::string serializeHeader(char type, int headerSize, int dataSize)
{
    int headerLength = 25;
    std::vector<char> header(headerLength, '*');

    // type of message; max size:1, offset 0
    header.insert( header.begin(), 1, type);

    // header size; max size: 12, offset: 1
    std::string s = std::to_string(headerSize);
    std::vector<char> v;
    std::copy( s.begin(), s.end(), std::back_inserter(v));
    for (int i=0; i<v.size(); i++)
        header[1+i] = v[i];

    
    // Data size; max size: 12, offset: 13
    std::string s2 = std::to_string(dataSize);
    std::vector<char> v2;
    std::copy( s2.begin(), s2.end(), std::back_inserter(v2));
    for (int i=0; i<v2.size(); i++)
        header[13+i] = v2[i];

    //std::cout << "header: " << header << std::endl;
    std::string s3(header.begin(), header.end());
    //std::cout << s3 << ", " << s3.size() << std::endl;

    return s3;
}


msg deserializeHeader(std::string s)
{
    msg h;

    std::vector<char> header;
    std::copy( s.begin(), s.end(), std::back_inserter(header));

    h.type = header[0];
    std::cout << h.type << std::endl;

    std::string s1 = s.substr(1,12);
    s1.erase(remove(s1.begin(), s1.end(), '*'), s1.end());
    h.headerSize = std::stoi(s1);

    std::string s2 = s.substr(13,12);
    s2.erase(remove(s2.begin(), s2.end(), '*'), s2.end());
    h.dataSize = std::stoi(s2);
    
    return h;
}


int main(int argc, char** argv) 
{
    std::string s = serializeHeader('d', 24500, 24349502);
    msg h = deserializeHeader(s);
    std::cout << "h: " << h.type << ", " << h.headerSize << ", " << h.dataSize << std::endl;

    return 0;
}