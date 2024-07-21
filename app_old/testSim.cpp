#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <random>

#include <mpi.h>
#include "seer.hpp"


int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cout << "json file needed address needed!" << std::endl;
        return 0;
    }

    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist15(1,5);


    MPI_Init(NULL, NULL);

    int world_size, world_rank;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    std::cout << "world size: " << world_size << ", rank: " << world_rank << std::endl;

    InSitu seer(world_rank, world_size, argv[1]);
    //seer.init("na+sm", argv[1], 123);
	//seer.init("ofi+tcp", argv[1], 123);

    int nElems = 50;
    int ts = 70;
    for (int t=0; t<ts; t++)
    {
        if (world_rank == 0)
            std::cout << "\n\nts: " << t << std::endl;

        MPI_Barrier(MPI_COMM_WORLD);
        
        // particles, let's not worry about topology for now
        {
            std::vector<float> value;
            for (int i=0; i<nElems; i++)
                value.push_back( i*(10*t) + world_rank*0.1 );
            std::cout << "pressure, numElements: " << value.size() << std::endl;

            seer.sendData(world_rank, t, "pressure_3", "data", "float", value.size(), &value[0]);
        }

        MPI_Barrier(MPI_COMM_WORLD);

        {
            std::vector<float> value;
            for (int i=0; i<nElems; i++)
                value.push_back(i*(10*t) + world_rank*0.02);
            std::cout << "temperature, numElements: " << value.size() << std::endl;

            seer.sendData(world_rank, t, "temperature_3", "data", "float", value.size(), &value[0]);
        }

        seer.tsDone(t);

        std::this_thread::sleep_for(std::chrono::seconds( dist15(rng)  ));
    }


    MPI_Finalize();

    return 0;
}


// -Ipusmochi-yokan-0.4.2-hiu7yh7om6nmyc2ahuknpdsov5k64zcj/include -I/vast/home/pascalgrosset/spack/opt/spack/linux-rhel8-haswell/gcc-9.4.0/mochi-margo-0.15.0-gbcextlqglkmsymdjchg7pornsleer6s/include
// -L/vast/home/pascalgrosset/spack/opt/spack/linux-rhel8-haswell/gcc-9.4.0/mochi-yokan-0.4.2-hiu7yh7om6nmyc2ahuknpdsov5k64zcj/lib  
// -lpyokan-client -lmargo
