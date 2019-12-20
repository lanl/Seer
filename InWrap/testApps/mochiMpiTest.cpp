#include <mpi.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mochi.h"

int main(int argc, char** argv)
{
    // Initialize the MPI environment
    MPI_Init(NULL, NULL);

    // Get the number of processes
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // Get the rank of the process
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    // Get the name of the processor
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int name_len;
    MPI_Get_processor_name(processor_name, &name_len);

    // Print off a hello world message
    std::cout << "Hello world from processor " << processor_name << " rank " << world_rank << " out of " << world_size<< " processors\n";

    mochiInterface mochi;
    //mochi.init("na+sm://22012/0", 1, "foo");

    //mochi.init("ofi+tcp://192.168.101.186:1234", 1, "foo2");
    std::cout << "argv[1]" << argv[1] << std::endl;
    mochi.init(argv[1], atoi(argv[2]), argv[3]);

    std::string key = "xxx_" + std::to_string(world_rank);
    std::string value = "xxyy_" + std::to_string(world_rank);

    mochi.putKeyValue(key, value);



    // Finalize the MPI environment.
    MPI_Finalize();
}

// mpicxx mpiTest.cpp -lmpi
// mpirun -np 2 ./testMPI ofi+tcp://192.168.101.186:1234 1 foo 
