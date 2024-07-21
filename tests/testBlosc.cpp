#include <blosc2.h>
#include <iostream>
#include <vector>
#include <cstdlib>

int main() 
{
  std::vector<float> data;
  for (int i=0; i<1000; i++)
    data.push_back(3.0);
  
  int NTHREADS = 2;
  blosc2_init();

  size_t dataSize = sizeof(data[0]) * data.size();
  std::cout << "dataSize: " << dataSize << std::endl;
  size_t osize = dataSize + BLOSC2_MAX_OVERHEAD;

  char * output = new char[dataSize]; //byte array;
	size_t csize = blosc1_compress(9, BLOSC_BITSHUFFLE, sizeof(data[0]), dataSize, &data[0], output, osize);
  std::cout << "Compressed size: " << csize << std::endl;

  if (csize < 0)
		throw std::runtime_error("Compression error. Error code: " + std::to_string(osize));

	if (csize > 0)
    output = (char *) realloc(output, csize);

  std::cout << "Compressed size: " << csize << std::endl;

  blosc2_destroy();

  return 0;
}


// -I/vast/home/pascalgrosset/spack/opt/spack/linux-rhel8-haswell/gcc-9.4.0/c-blosc2-2.11.1-rkb63dt4yt5xzwamtpdhixbvbmgbwskm/include
// -L/vast/home/pascalgrosset/spack/opt/spack/linux-rhel8-haswell/gcc-9.4.0/c-blosc2-2.11.1-rkb63dt4yt5xzwamtpdhixbvbmgbwskm/lib64
// -lblosc2

