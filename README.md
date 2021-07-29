<img src="Seer_Logo.png" width="200">

Seer is a lightweight insitu wrapper library adding insitu capabilities to simulations.


## Requirements

* CMake 3.10 or above
* C++ 11
* MPI 3
* Mochi
* Paraview Catalyst


## Environment Setup

* This project uses [Spack](https://spack.readthedocs.io/en/latest/). Once Spack is installed, modify (or create) packages.yaml to contain the following:

~~~bash
packages:
    libfabric:
        variants: fabrics=tcp,rxm
~~~

* Setting up packages

~~~bash
spack install openmpi

# Mochi
git clone https://github.com/mochi-hpc/mochi-spack-packages
spack repo add mochi-spack-packages
spack install mochi-margo
spack install mochi-sdskv+leveldb
spack install py-mochi-sdskv

# To install Jupyter notebook (for the client)
#   load the python associated with the mochi python
spack load -r py-sdskv  

#   install jupyter for that python
curl https://bootstrap.pypa.io/get-pip.py -o get-pip.py
python get-pip.py
python -m pip install jupyter

# ParaView
spack install paraview@5.7.0 +osmesa +python3 ^hwloc@1.9

# VTK (Optional for testing)
spack install vtk #(spack install vtk ^hdf5+hl+mpi to bypass error)

# Papi (usually already on the server and doesn't need install)
spack install papi
~~~


## Building the sim

The following environment needs to be activated as follows:

~~~bash
# load whatever modules the sim needs

# load Seer insitu stuff as follows
spack load -r margo
spack load -r sdskeyval

spack load paraview@5.7.0
spack load mesa #needed for ParaView without X

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
spack load -r margo
spack load -r sdskeyval

# Distributed memory
# skv-server-daemon ofi+tcp://<path of server>:<port of server> <name of db>:ldb &
sdskv-server-daemon ofi+tcp://192.168.101.186:1234 foo_test1:ldb &

# Shared memory (testing purposes)
sdskv-server-daemon na+sm foo:ldb -f address &
~~~


### 2. Run the Sim (usually batch script)

~~~bash
# load whatever modules the sim needs

# load Seer insitu stuff as follows 
spack load -r margo
spack load -r sdskeyval

spack load paraview@5.7.0
spack load mesa #needed for ParaView without X


# Run the sim

# Distributed memory
# mpirun -np 4 <sim_name> --insitu <input file>
mpirun -np 4 demoApps/miniAppStructured --insitu ../inputs/input-test.json  

# Shared memory (testing purposes
demoApps/testMPI na+sm://9923/0 1 foo 10  
~~~

### 3. Run the client

#### Remote (On Server)

Get a compute node on the server

~~~bash
# Load the modules
spack load -r py-sdskv

# only needed first time
jupyter-notebook password

# Launch jupyter notebook on the server
# jupyter-notebook --no-browser --port=<port_number> --ip=0.0.0.0
jupyter-notebook --no-browser --port=8897 --ip=0.0.0.0

~~~

#### Local (client)

~~~bash
# Tunnel to the server
#   ssh -N -f -L <port_number>:<host_name>:<port_number> username@cluster 
ssh -N -f -L 8897:cn37:8897 pascalgrosset@darwin-fe
~~~

In the browser:

~~~bash
# http://localhost:<port_number>
http://localhost:8897
~~~

# Note

## Environment setup

Scripts for setting up the environment different platforms are located in the evn folder:

* <machine_name>_sim.sh
* <machine_name>_mochiServer.sh
* <machine_name>_jupyter.sh

Scripts for launching sims are located in the script folder:

* miniAppStruc_darwin.sh
* runBatch__256_16_16_scaling_mochi.sh
* runBatch__64_8_8_glaton_mochi.sh


## No papi counters found

* Check if papi events are around using: papi_avail | more
* Turn them on (if disabled) using: sudo sh -c 'echo 1 >/proc/sys/kernel/perf_event_paranoid'


# Citing Seer
Pascal Grosset, Jesus Pulido, James Ahrens, "Personalized In Situ Steering for Analysis and Visualization",  In Proceedings of the Workshop on In Situ Infrastructures for Enabling Extreme-Scale Analysis and Visualization (ISAV '20). Association for Computing Machinery, New York, NY, USA. DOI:https://doi.org/10.1145/3426462.3426463

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
