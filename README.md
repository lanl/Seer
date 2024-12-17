## Steps

### Start a server

Allocate a node and start a server as follows:
```
salloc -N 1 -p scaling

source env/loadEnv.sh                                           # setup the environment
source run/launchMochiServer.sh run/mochi-yokan-config.json     # launch the server
```

The server will be launched and an address will be displayed on the terminal e.g.
```
[pascalgrosset@cn345 run]$ source launchMohiServer.sh mochi-yokan-config.json 
[pascalgrosset@cn345 run]$ [2024-06-27 14:15:37.860] [info] [yokan] YOKAN provider registration done
[2024-06-27 14:15:37.861] [info] Bedrock daemon now running at ofi+tcp://192.168.81.80:389
```
Copy the address '192.168.81.80:389' to the mochi-yokan-config.json file in address.


### Build and run sim

Allocate another node for the sim. To build:

```
cd build
ccmake ../src
```

Set the location of BLOSC path and include
e.g.
```
/vast/home/pascalgrosset/spack/opt/spack/linux-rhel8-haswell/gcc-9.4.0/c-blosc2-2.11.1-rkb63dt4yt5xzwamtpdhixbvbmgbwskm/include
/vast/home/pascalgrosset/spack/opt/spack/linux-rhel8-haswell/gcc-9.4.0/c-blosc2-2.11.1-rkb63dt4yt5xzwamtpdhixbvbmgbwskm/lib64/libblosc2.so
```


To test run:
```
build/testSim run/mochi-yokan-config.json
```

### Client access
On a server, using the same node as the server is fine, do: 
```
run/launchJupyterNotebookServer.sh <port>
```

It will output intructions on how to connect from the client, e.g.:
```
On the client do:
ssh -N -f -L 8871:cn342:8871 pascalgrosset@darwin-fe
then in a browser: http://localhost:8871
```