#include <iostream>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <thallium.hpp>
#include <mpi.h>

#include "utils.hpp"

namespace tl = thallium;

int main(int argc, char** argv) 
{
    MPI_Init(NULL, NULL);
    int world_size;
    int world_rank = 3;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    std::stringstream str_strm;
    int num_elements = 10+world_rank;
    str_strm << num_elements;
    std::string buffer1 = str_strm.str();
    //std::cout << "buffer: " << buffer1 << std::endl;
    std::cout << buffer1.size() << std::endl;


    std::vector<float> data;
    std::stringstream ss;
    for (int i=0; i<num_elements; i++)
        data.push_back(world_rank);

    std::string serializedString = serializeData(num_elements, data);
    std::cout << "serializedString: " << serializedString << std::endl;

    std::vector<float> xx = deserializeData(num_elements, serializedString);
    for (int i=0; i<num_elements; i++)
        std::cout << xx[i] << " ";
    std::cout << std::endl;



    std::copy( data.begin(), data.end(), std::ostream_iterator<int>(ss, " "));
    std::string dataBuffer = ss.str();
    std::cout << dataBuffer.size() << std::endl;
    
    tl::engine myEngine("tcp", MARGO_CLIENT_MODE);
    tl::remote_procedure remote_do_rdma = myEngine.define("do_rdma");
    tl::endpoint server_endpoint = myEngine.lookup("tcp://127.0.0.1:1234");

    // //std::string buffer = "Matthieu";
    std::vector<std::pair<void*,std::size_t>> segments(1);
    segments[0].first  = (void*)(&buffer1[0]);
    segments[0].second = buffer1.size()+1;

    // segments[1].first  = (void*)(&dataBuffer[0]);
    // segments[1].second = dataBuffer.size()+1;

    tl::bulk myBulk = myEngine.expose(segments, tl::bulk_mode::read_only);

    remote_do_rdma.on(server_endpoint)(myBulk);
    


    //std::cout << buffer.size() << std::endl;

    MPI_Finalize();

    return 0;
}




