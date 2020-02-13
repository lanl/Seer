# InWrap
InWrap is a lightweight insitu wrapper library adding insitu capabilities to simulations.

## Requirements:
 * CMake 3.10 or above
 * C++ 11
 * MPI 3
 * Mochi

### Optional Requirements:
 * [Sensei](https://github.com/Kitware/sensei)
 * [Catalyst](https://www.paraview.org/files/v5.5/Catalyst-v5.5.2-Base-Enable-Python-Essentials-Extras-Rendering-Base.tar.gz)


## Env Setup
 *  This project uses [Spack](https://spack.readthedocs.io/en/latest/). Once Spack is installed, modify (or create) packages.yaml to contain the following:
	~~~bash
	packages:
        	libfabric:
            	variants: fabrics=tcp,rxm
	~~~

 * Setting up Mochi
	~~~bash
	spack create env inwrap
	git clone https://xgitlab.cels.anl.gov/sds/sds-repo.git
	spack repo add sds-repo
	spack install margo
	~~~~

 * Others (sdskeyval, py-sdskv)
	~~~bash
	spack install sdskeyval+bdb+leveldb
	spack install py-sdskv
	spack install vtk (spack install vtk ^hdf5+hl+mpi to bypass error)
	~~~

 * On a PC, also do this:
	~~~bash
	spack install openmpi
	spack install cmake
	~~~


## Building
The following environment needs to be activated as follows:

~~~bash
spack env activate inwrap
spack load -r margo
spack load -r sdskeyval
spack load paraview@5.7.0
spack load /6uyrlxs #mesa
~~~

~~~bash
cd src
mkdir build
cd build
ccmake ..
~~~


## Running
There are two parts of running the insitu package, i) running the sim, and ii) running the client

### Env setup to run sim
~~~bash
spack env activate inwrap
spack load -r /ayohgie #margo
spack load -r sdskeyval
spack load paraview@5.7.0
spack load /6uyrlxs #mesa
~~~

### Env setup to run python client
~~~bash
spack env activate inwrap
spack load -r py-sdskv
~~~


#### server:
~~~bash
sdskv-server-daemon na+sm foo:ldb -f address &                # shared mem
sdskv-server-daemon ofi+tcp://192.168.101.186:1234 foo2:ldb   # distributed mem
~~~

#### client:
~~~bash
demoApps/testMPI na+sm://9923/0 1 foo 10                                    # shared mem
mpirun -np 4 demoApps/miniAppStructured --insitu ../inputs/input-test.json  # distributed mem
~~~


#### Jupyter:

* Remote:
~~~bash
jupyter password: password	                            # if needed
jupyter-notebook --no-browser --port=8898 --ip=0.0.0.0
~~~

* Local:  
ssh -N -f -L port:host:hostport username@cluster e.g.
~~~bash
ssh -N -f -L 8898:cn36:8898 pascalgrosset@darwin-fe
~~~

In Browser:
~~~bash
http://localhost:8898
~~~


## Adding runtime options using Mochi
Using Mochi, we can pass information to the sim while it is running. Keywords are as follows:
 * Key: ADD_PAPI_COUNTER, 	 Value: <PAPI_COUNTER_NAME> 	- add a new PAPI Counter
 * Key: REMOVE_PAPI_COUNTER, Value: <PAPI_COUNTER_NAME> 	- remove an existing PAPI counter
 * Key: NEW_<ENTRY_TYPE>,	 Value: <Entry_Name>
 * Key: NEW_ENTRIES,		 Value: 


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
