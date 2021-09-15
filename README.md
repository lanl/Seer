# Seer

Seer is a lightweight insitu wrapper library adding insitu capabilities to simulations.


## Requirements

Server-Side:
* CMake 3.10 or above
* C++ 11
* MPI 3
* Mochi

Note: gcc/6.4.0 is recommended (gcc/7*, gcc/8*, gcc/9* caused errors when installing Mochi packages ... :( )

Client-Side:
 * dash
 * pandas
 * plotly
 * scp
 * paramiko


# Setup

These are need for the first time only at setup:

## Server Side

This project uses [Spack](https://spack.readthedocs.io/en/latest/).
~~~bash
git clone https://github.com/spack/spack.git
spack/share/spack/setup-env.sh
spack install gcc@6.4.0
~~~
modify (or create) packages.yaml in the ~/.spack folder so that it contains the following:


~~~bash
packages:
    libfabric:
        variants: fabrics=tcp,rxm
~~~

### Setting up packages:

~~~bash
# Mochi
git clone https://github.com/mochi-hpc/mochi-spack-packages
spack repo add mochi-spack-packages
spack install mochi-margo ^libfabric@1.11.0     # there appears to be a bug in the latest libfrabic 1.13.0
spack install mochi-sdskv+leveldb ^libfabric@1.11.0
spack install py-mochi-sdskv ^libfabric@1.11.0
spack install papi
~~~

Will take about two hours or so ...

For simplicity, create an bash file (e.g. evn.sh) which contains the following, which will be sourced anytime the modules are needed.
~~~bash
spack load gcc@6.4.0
spack load cmake
spack load -r mochi-margo
spack load -r mochi-sdskv+leveldb
spack load -r py-mochi-sdskv
spack load -r papi
~~~


## Client side

Only python 3.7+ is needed on the client. [Miniconda](https://docs.conda.io/en/latest/miniconda.html) is a good package to install.
~~~bash
pip install paramiko
pip install scp
pip install dash
pip install dash_bootstrap_components
pip install pandas
~~~

# Building the sim

The following environment needs to be activated as follows:

~~~bash
# load Seer insitu stuff as follows or use the evn.sh file created
spack load gcc@6.4.0
spack load cmake
spack load -r mochi-margo
spack load -r mochi-sdskv+leveldb
spack load -r py-mochi-sdskv
spack load -r papi
~~~

or

~~~ bash
source evn/env_darwin_new.sh
~~~


## Build the code:

~~~ bash
cd src
mkdir build

cd build
ccmake ..
~~~



# Running

There are three parts of running the insitu package

1. launch the mochi server
2. run the sim
3. run the client


### 1. Run the Mochi Server (done on server, usually in a batch script)
Find the ipaddress of the node using $ifconfig$ and add that to the address field in inputs/input-test.json.
That ipaddress will also be used to launch the mochi server.


~~~bash
# Load the modules (if not previously loaded)
spack load -r mochi-margo
spack load -r mochi-sdskv+leveldb
# or
source evn/env_darwin_new.sh

# Distributed memory
# sdskv-server-daemon ofi+tcp://<path of server>:<port of server> <name of db>:ldb &
sdskv-server-daemon ofi+tcp://192.168.101.180:1234 foo_test2:ldb &


# Shared memory (testing purposes on local machine only)
sdskv-server-daemon na+sm foo_test1:ldb -f address

e.g. sdskv-server-daemon na+sm foo_test1:ldb -f address
~~~

Then open the file called "address" and locate the na+sm address in there. It could be something like "na+sm://1107352/0".
This address is then used in the ["mochi-database"]["address"] field in the JSON file that is passed to the simulation.


### 2. Run the Sim (done on the server, usually batch script)

~~~bash
# load whatever modules the sim needs (if not previously loaded)

# load Seer insitu stuff as follows 
spack load -r mochi-margo
spack load -r mochi-sdskv+leveldb
# or
source evn/env_darwin_new.sh


# Run the sim

# Distributed memory
# mpirun -np 4 <sim_name> --insitu <input file>
mpirun -np 4 demoApps/miniAppStructured --insitu ../inputs/input-test.json  

# Shared memory (testing purposes only)
demoApps/testMPI na+sm://9923/0 1 foo 10  
~~~

<strong>Note:</strong> Steps The Mochi Server and the Sim can be on the same node


### 3. Run the client (on the local desktop)
~~~bash
python Seer_dash.py
~~~
<!-- 
#### Remote (On Server)

~~~bash
# Load the modules
spack load -r py-mochi-sdskv

~~~

#### Local (client)

~~~bash
# Tunnel to the server
#   ssh -N -f -L <port_number>:<host_name>:<port_number> username@cluster 
ssh -N -f -L 8897:cn30:8897 pascalgrosset@darwin-fe
~~~

In the browser:

~~~bash
# http://localhost:<port_number>
http://localhost:8897
~~~ -->





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


