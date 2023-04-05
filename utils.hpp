#pragma once

#include <string>
#include <sstream>
#include <vector>


inline std::string serializeData(size_t n, std::vector<float> data)
{
    // Note: Only to be used in the prototype stage. Will need to be replaced!

    std::stringstream ss;
    for (size_t i=0; i<data.size()-1; i++)
        ss << data[i] << " ";
    ss << data[ data.size()-1 ];

    return ss.str();
}


inline std::vector<float> deserializeData(size_t n, std::string str)
{
    // Note: Only to be used in the prototype stage. Will need to be replaced!
    
    std::vector<float> result;
    float number;

    std::stringstream ss(str); 
    while (ss >> number) 
        result.push_back(number);

    return result;
}