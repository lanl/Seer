There are the steps to setup and run SeerX with a dummy application:

## Setup Environment
setup spack as descriped in https://spack.readthedocs.io/en/latest/getting_started.html


modify (or create) packages.yaml and spack.yaml (usually in ~/.spack) to contain the following:
```
packages:
    libfabric:
        variants: fabrics=tcp,rxm
```


Load compilers on your HPC, e.g.
```
module load openmpi/3.1.6-gcc_9.4.0
module load cmake/3.29.2
```


Load and create a spack environment as follows:
```
source loadSpack.sh                         # Load spack
spack env create vevn_spack_seerx2
spack env activate vevn_spack_seerx2
````

modify (or create) packages.yaml and spack.yaml (usually in ~/.spack) to contain the following in the spack environment,
e.g. in /vast/home/pascalgrosset/software/spack/var/spack/environments/vevn_spack_seerx2
```
[pascalgrosset@cn340 vevn_spack_seerx2]$ cat packages.yaml 
packages:
  libfabric:
        variants: fabrics=rxm,tcp
```

Get the latest mochi packages and add it to spack:
```
spack compilers                         # Make sure compilers are there
git clone https://github.com/mochi-hpc/mochi-spack-packages.git
spack repo add mochi-spack-packages
```

Add packages to the enviornment and install them:
```
spack add python
spack add c-blosc2
spack add zstd
spack add nlohmann-json
spack add mochi-margo         
spack add mochi-bedrock
spack add mochi-yokan +python

# Install spack packages
spack install -j python
spack install -j16 c-blosc2
spack install -j16 zstd
spack install -j16 nlohmann-json
spack install -j16 mochi-margo
spack install -j16 mochi-bedrock
spack install -j16 mochi-yokan +python

# Activate spack packages
spack load python
spack load mochi-yokan+python
spack load mochi-margo
spack load mochi-bedrock
spack load c-blosc2
spack load zstd
spack load nlohmann-json
```

Create and acticate python virtual environment
```
python -m venv venv_seerx
source venv_seerx/bin/activate
```

Add packages to it:
```
python -m pip install jupyterlab
python -m pip install numpy
```

Register jupyterlab to the environment:
```
python -m ipykernel install --user --name=venv_seerx
```

find the location of blosc, mochi-yokan, and mochi margo as follows:
spack location -i <spack package_name>

e.g.
```
spack location -i c-blosc2
/vast/home/pascalgrosset/software/spack/opt/spack/linux-broadwell/c-blosc-1.21.6-rl2gipraqcdsv3ar7tttojw6op2gapqp
```

## Start a server

Allocate a node and start a server as follows:
```
salloc -N 1 -p scaling

source env/loadEnv.sh                                           # setup the environment 
source run/launchMohiServer.sh run/mochi-yokan-config.json &    # launch the server
```

The server will be launched and an address will be displayed on the terminal e.g.
```
[pascalgrosset@cn345 run]$ source launchMohiServer.sh mochi-yokan-config.json 
[pascalgrosset@cn345 run]$ [2024-06-27 14:15:37.860] [info] [yokan] YOKAN provider registration done
[2024-06-27 14:15:37.861] [info] Bedrock daemon now running at ofi+tcp://192.168.81.80:389
```
Copy the address '192.168.81.80:389' to the mochi-yokan-config.json file for the application e.g. in simple_app/mochi-yokan-config.json


### Build and run sim

Allocate another node for the sim. 

To build:
```
source env/loadEnv.sh       # setup the environment 
cd simple_app                                    
source buildScript.sh
```

To run:
```
source env/loadEnv.sh        # setup the environment 
mpirun -np 2 simple_app/benchmarkSim simple_app/mochi-yokan-config.json
```


### Client access
On a server, using the same node as the server is fine, do: 
```
source client/launchJupyterLabServer.sh <port> &     # e.g. source client/launchJupyterLabServer.sh 8871 &
```

It will output intructions on how to connect from the client, e.g.:
```
On the client do:
ssh -N -f -L 8871:cn342:8871 pascalgrosset@darwin-fe
then in a browser: http://localhost:8871
```