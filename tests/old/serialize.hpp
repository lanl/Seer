#pragma once

#include <blosc2.h>
#include <string>
#include <stringstream>

template <class T>
inline std::string serializeData(T* data, int numElems)
{
    
    return ss.str();
}

template <class T>
inline char * compress(T* data, size_t numElems, size_t & csize)
{
    int compressionLevel = 9;
    blosc2_init();

    size_t dataSize = sizeof(data[0]) * numElems;
    size_t osize = dataSize + BLOSC2_MAX_OVERHEAD;

    char * output = new char[dataSize]; //byte array;
	csize = blosc1_compress(compressionLevel, BLOSC_BITSHUFFLE, sizeof(data[0]), dataSize, &data[0], output, osize);
    //std::cout << "Compressed size: " << csize << std::endl;

    if (csize < 0)
		throw std::runtime_error("Compression error. Error code: " + std::to_string(csize));

	if (csize > 0)
        output = (char *) realloc(output, csize);

    std::cout << "Compressed size: " << csize << std::endl;

    blosc2_destroy();

    return output;
}
