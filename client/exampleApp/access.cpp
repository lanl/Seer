#include <mpi.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <vector>

#include "seer_client.hpp"

int main(int argc, char** argv) 
{    
    MPI_Init(NULL, NULL);
    
    int world_size, world_rank;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);    
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);


    std::string jsonFilename = std::string(argv[1]);
    std::ifstream jsonFile(jsonFilename);
    nlohmann::json jsonInput;
    jsonFile >> jsonInput;

    std::string simid = jsonInput["sim-id"];
    std::vector<std::string> fields;
    for (int i=0; i<jsonInput["data"].size(); i++)
    {
        fields.push_back( jsonInput["data"][i]["name"]);
    }


    SeerClient seer;
    seer.init(jsonFilename);

    int ts = 0;
    while(true)
    {
        if (seer.isTimestepReady(ts))
        {
            std::string key_prefix = "_" + simid + "/" + std::to_string(ts) + "/" + std::to_string(world_rank) + "/";

            size_t numElements;
            float *pressure = seer.getData(ts,world_rank, "pressure_3", numElements);
            float *temperature = seer.getData(ts,world_rank, "temperature_3", numElements);

            std::cout << "\nPressure ~ ts:" << ts << " num elemenst: " << numElements << std::endl;
            for (int i=0; i<19; i++)
            {
                std::cout << pressure[i] << ", ";
            }
            std::cout << "\n";


            std::cout << "\ntemperature ~ ts:" << ts << " num elemenst: " << numElements << std::endl;
            for (int i=0; i<19; i++)
            {
                std::cout << temperature[i] << ", ";
            }
            std::cout << "\n";

            delete []pressure;
            delete []temperature;
        }

        ts++;
    }
    
    
    // Finalize the MPI environment.
    MPI_Finalize();
}