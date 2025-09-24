#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <random>

#include <mpi.h>
#include "seer_o.hpp"
#include "utils.hpp"

#include <bits/stdc++.h>

float randomFloat()
{
    return (float)(std::rand()) / (float)(std::rand());
}


int main(int argc, char *argv[])
{
    std::stringstream debugLog;

    if (argc < 2)
    {
        std::cout << "json file needed address needed!" << std::endl;
        return 0;
    }


    std::ifstream jsonFile(argv[1]);
    nlohmann::json jsonInput;
    jsonFile >> jsonInput;
    std::string simID = jsonInput["sim-id"];
    int ts =  jsonInput["benchmark"]["num-ts"];
    int nElems =  jsonInput["benchmark"]["num-elements"];


    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist15(1,30);


    MPI_Init(NULL, NULL);

    int world_size, world_rank;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    std::cout << "world size: " << world_size << ", rank: " << world_rank << std::endl;

    InSitu seer;
    std::this_thread::sleep_for(std::chrono::milliseconds(1500 * world_rank));
    seer.init(world_rank, world_size, argv[1]);
    //seer.init("na+sm", argv[1], 123);
	//seer.init("ofi+tcp", argv[1], 123);

    std::cout << ", rank: " << world_rank << std::endl;
    MPI_Barrier(MPI_COMM_WORLD);
     std::cout << world_rank << ", " << world_size << std::endl;

    // int nElems = 20000000;
    // int ts = 50;
    for (int t=0; t<ts; t++)
    {
        std::cout << world_rank << " ~ ts: " << t << std::endl;
        if (world_rank == 0)
            std::cout << "\n\nts: " << t << std::endl;

        //MPI_Barrier(MPI_COMM_WORLD);
        
        // particles, let's not worry about topology for now
        {
            std::cout << "pressure_3. numElements: " << nElems << std::endl;
            std::vector<float> value;
            for (int i=0; i<nElems; i++)
            {
                value.push_back( randomFloat() );
                //value.push_back( 50000 + i*(10*t) + world_rank*0.0001 );
                //debugLog << 50000 + i*(10*t) + world_rank*0.0001 << "," << std::endl;
            }
            seer.sendData(world_rank, t, "pressure_3", "data", "float", value.size(), &value[0]);
        }

        debugLog <<  "\n" << std::endl;

        {
            std::cout << "temperature_3, numElems: " << nElems << std::endl;
            std::vector<float> value;
            for (int i=0; i<nElems; i++){
                value.push_back( randomFloat() );
                //value.push_back(10000 + i*(10*t) + world_rank*0.02);
                //debugLog << 10000 + i*(10*t) + world_rank*0.02 << "," << std::endl;
            }
            seer.sendData(world_rank, t, "temperature_3", "data", "float", value.size(), &value[0]);
        }



        debugLog <<  "\n" << std::endl;

        {
            std::cout << "energy_3, numElems: " << nElems << std::endl;
            std::vector<float> value;
            for (int i=0; i<nElems; i++){
                value.push_back( randomFloat() );
                //value.push_back(10000 + i*(10*t) + world_rank*0.02);
                //debugLog << 10000 + i*(10*t) + world_rank*0.02 << "," << std::endl;
            }
            seer.sendData(world_rank, t, "energy_3", "data", "float", value.size(), &value[0]);
        }
        

        seer.tsDone(t);
        debugLog <<  "\n--------------------------\n" << std::endl;

        //std::this_thread::sleep_for(std::chrono::seconds( dist15(rng) ));
    }

    seer.simDone();
    //MPI_Finalize();

    writeLog( ("sim_" + simID + "_" + std::to_string(world_rank)),  debugLog.str());

    return 0;
}


// -Ipusmochi-yokan-0.4.2-hiu7yh7om6nmyc2ahuknpdsov5k64zcj/include -I/vast/home/pascalgrosset/spack/opt/spack/linux-rhel8-haswell/gcc-9.4.0/mochi-margo-0.15.0-gbcextlqglkmsymdjchg7pornsleer6s/include
// -L/vast/home/pascalgrosset/spack/opt/spack/linux-rhel8-haswell/gcc-9.4.0/mochi-yokan-0.4.2-hiu7yh7om6nmyc2ahuknpdsov5k64zcj/lib  
// -lpyokan-client -lmargo
