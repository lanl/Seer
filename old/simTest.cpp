#include <vector>
#include <iostream>
#include <string>
#include <map>

#include "data.hpp"


int main(int argc, char** argv) 
{
    int myRank = 0;
    int numRanks = 8;

    int n = 1000;
    int numPoints = 50;

    for (int i=0; i<n; i++)
    {
        // position
        std::vector<float> x(numPoints);
        std::vector<float> y(numPoints);
        std::vector<float> z(numPoints);

        // data
        std::vector<float> temp(numPoints);


        
        SimData simData;

        Record x,y,z, temp;
        x.info["rank"] = myRank;
        x.info["world-size"] = numRanks;
        x.info["type"] = "float";
        x.info["association"] = "vertex";
        x.info["elem-size"] = 1;
        x.info["total-num-elems"] = numPoints;
        x.setData(&x[0], numPoints);

        simData.push_back(x);

        y = x;
        y.setData(&y[0], numPoints);
        simData.push_back(y);

        z = x;
        z.setData(&y[0], numPoints);
        simData.push_back(z);

        



        simData["num_vars"] = 4;
        simData["ts"] = i;
        simData["total_ts"] = n;



        simData["temp/data"] = "float";
        simData["temp/association"] = "vertex";
        simData["temp/arrangement"] = "scalar";


    }

    return 0;
}