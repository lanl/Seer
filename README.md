# Seer

Seer is a lightweight insitu wrapper library adding insitu capabilities to simulations.


## Requirements:

* CMake 3.10 or above
* C++ 11
* MPI 3
* Mochi


### Optional Requirements

* [Sensei](https://github.com/Kitware/sensei)
* [Catalyst](https://www.paraview.org/files/v5.5/Catalyst-v5.5.2-Base-Enable-Python-Essentials-Extras-Rendering-Base.tar.gz)


## Env Setup

* This project uses [Spack](https://spack.readthedocs.io/en/latest/). Once Spack is installed, modify (or create) packages.yaml to contain the following:

~~~bash
packages:
    libfabric:
        variants: fabrics=tcp,rxm
~~~

* Setting up Mochi and VTK

~~~bash
git clone https://xgitlab.cels.anl.gov/sds/sds-repo.git
spack repo add sds-repo
spack install margo
spack install sdskeyval+bdb+leveldb
spack install py-sdskv
spack install openmpi
spack install cmake
spack install vtk (spack install vtk ^hdf5+hl+mpi to bypass error) # VTK
~~~

If linking to catalyst, ParaView needs to be installed as well


## Building

The following environment needs to be activated as follows:

~~~bash
spack load -r margo
spack load -r sdskeyval

spack load paraview@5.7.0
spack load /6uyrlxs #mesa needed for ParaView without X

cd src
mkdir build
cd build
ccmake ..
~~~


## Running

There are two parts of running the insitu package, i) running the sim, and ii) running the client


### Env setup to run sim

~~~bash
spack load -r /ayohgie #margo
spack load -r sdskeyval

spack load paraview@5.7.0
spack load /6uyrlxs #mesa needed for ParaView
~~~

### Env setup to run python client

~~~bash
spack load -r py-sdskv
~~~


#### Launch Mochi server remotely

~~~bash
sdskv-server-daemon na+sm foo:ldb -f address &                		# for shared mem
sdskv-server-daemon ofi+tcp://192.168.101.186:1234 foo_test1:ldb &  # for distributed mem
~~~


#### Run Sim remotely

~~~bash
demoApps/testMPI na+sm://9923/0 1 foo 10                                    # shared mem
mpirun -np 4 demoApps/miniAppStructured --insitu ../inputs/input-test.json  # distributed mem
~~~


#### Jupyter

* Remote

~~~bash
module load anaconda/Anaconda3 
jupyter-notebook password                  # only needed first time
jupyter-notebook --no-browser --port=8897 --ip=0.0.0.0
~~~

* Local

ssh -N -f -L port:host:hostport username@cluster e.g.

~~~bash
ssh -N -f -L 8897:cn36:8897 pascalgrosset@darwin-fe
~~~

In Browser

~~~bash
http://localhost:8897
~~~


## Adding runtime options using Mochi

Using Mochi, we can pass information to the sim while it is running. Keywords are as follows:

* Key: PAPI:ADD, Value: <PAPI_COUNTER_NAME> - add a new PAPI Counter
* Key: PAPI:DEL, Value: <PAPI_COUNTER_NAME> - remove an existing PAPI counter


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
