<img src="Seer_Logo.png" width="200">

Seer is a lightweight insitu wrapper library adding insitu capabilities to simulations.


## Requirements

* CMake 3.10 or above
* C++ 11 (gcc 9.4 or higher)
* MPI 3
* Mochi


## Environment Setup

* This project uses [Spack](https://spack.readthedocs.io/en/latest/). Once Spack is installed, modify (or create) packages.yaml, usually in ~/.spack to contain the following:

~~~bash
packages:
    libfabric:
        variants: fabrics=tcp,rxm
~~~

* Setting up packages

~~~bash

# Mochi
git clone https://github.com/mochi-hpc/mochi-spack-packages.git
spack repo add mochi-spack-packages

spack install mochi-yokan@0.4.2
spack install py-blosc2
spack install py-notebook
spack install py-matplotlib
spack install py-notebook
spack install nholmann-json
spack install c-blosc2
~~~


## Building the sim

The following packages need to be loaded. It's easier to put them in a scipt that can be run, e.f. loadEnv.sh.

~~~bash
# load the modules

module load openmpi/3.1.6-gcc_9.4.0
module load cmake

spack load mochi-yokan@0.4.2 
spack load conduit
spack load py-blosc2
spack load py-notebook
spack load py-numpy
spack load py-matplotlib
spack load c-blosc2
spack load nholmann-json


cd src
mkdir build

cd build
ccmake ..
~~~


## Running

There are three parts of running the insitu package

1. launch the mochi server
2. run the sim
3. run the client


### 1. Run the Mochi Server (done on server, usually in a batch script)

~~~bash
# Load the modules
source loadEnv.sh

# Distributed memory
source lauchMochiServer.sh mochi-yokan-config.json
~~~


### 2. Run the Sim (usually batch script)

~~~bash
# load whatever modules the sim needs
source loadEnv.sh

# Run the sim

# Distributed memory
mpirun -np 4 sim_test mochi-yokan-config.json  
~~~

### 3. Run the client

#### Remote (On Server)

Get a compute node on the server

~~~bash
# load whatever modules the sim needs
source loadEnv.sh

# only needed first time
souce lauchJupyterNotebookServer.sh <port e.g. 8871>

# Look for the command to use to connect from the client
~~~

#### Local (client)

~~~bash
# Tunnel to the server
# USe the command from above, it should look like:
#   ssh -N -f -L <port_number>:<host_name>:<port_number> username@cluster 
ssh -N -f -L 8897:cn37:8897 pascalgrosset@darwin-fe
~~~

In the browser:

~~~bash
# http://localhost:<port_number>
http://localhost:8897
~~~

# Citing Seer
Pascal Grosset, Jesus Pulido, James Ahrens, "Personalized In Situ Steering for Analysis and Visualization",  In Proceedings of the Workshop on In Situ Infrastructures for Enabling Extreme-Scale Analysis and Visualization (ISAV '20). Association for Computing Machinery, New York, NY, USA. DOI:https://doi.org/10.1145/3426462.3426463

Pascal Grosset and James Ahrens. 2021. Lightweight Interface for In Situ Analysis and Visualization of Particle Data. In ISAV'21: In Situ Infrastructures for Enabling Extreme-Scale Analysis and Visualization (ISAV'21). Association for Computing Machinery, New York, NY, USA, 12–17. DOI:https://doi.org/10.1145/3490138.3490143

The Seer paper is available at: https://permalink.lanl.gov/object/tr?what=info:lanl-repo/lareport/LA-UR-20-26538

# Copyright

© (or copyright) 2019. Triad National Security, LLC. All rights reserved.

This program was produced under U.S. Government contract 89233218CNA000001 for Los Alamos
National Laboratory (LANL), which is operated by Triad National Security, LLC for the U.S.
Department of Energy/National Nuclear Security Administration. All rights in the program are
reserved by Triad National Security, LLC, and the U.S. Department of Energy/National Nuclear
Security Administration. The Government is granted for itself and others acting on its behalf a
nonexclusive, paid-up, irrevocable worldwide license in this material to reproduce, prepare
derivative works, distribute copies to the public, perform publicly and display publicly, and to permit
others to do so.
