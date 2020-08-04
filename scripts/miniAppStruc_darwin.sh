#!/bin/bash
#SBATCH -N 2
#SBATCH --ntasks-per-node 1
#SBATCH -p galton

export MPIP="-t 10.0"
projectpath=/projects/insituperf/Seer
#pythonExe=/home/pascalgrosset/miniconda3/bin/python

# init spack
export SPACK_ROOT=$HOME/spack
source $SPACK_ROOT/share/spack/setup-env.sh

# load modules
source $projectpath/evn/env_darwin_sim.sh


module list

# Get IP
nodes=$(scontrol show hostnames $SLURM_JOB_NODELIST) # Getting the node names
nodes_array=( $nodes )
node1=${nodes_array[0]}

ip=$(srun --nodes=1 --ntasks=1 -w $node1 hostname --ip-address)
echo $ip

cd $projectpath/buildII
mkdir logs


# Create 
mpirun python $projectpath/Seer_Mochi/createJsonConfig.py $ip $projectpath/inputs/input-test-structured.json

echo "create Json done"

# Execute jobs in parallel: server + demoApp
mpirun python $projectpath/Seer_Mochi/launchMochiServer.py $projectpath/inputs/input-test-structured.json &
mpirun $projectpath/buildII/demoApps/miniAppStructured --insitu $projectpath/inputs/input-test-structured.json

## Terminate the server
#mpirun $pythonExe $projectpath/InWrap/python-utils/shutDownServer.py $projectpath/inputs/input-test-structured.json 