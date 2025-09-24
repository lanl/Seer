#!/bin/bash
#SBATCH -N 1
#SBATCH --ntasks-per-node 1
#SBATCH -p scaling
#SBATCH --time=04:59:59

source /vast/home/pascalgrosset/loadSpack.sh
source /vast/projects/exasky/pascal/HACC/Seer/env/loadEnv.sh


# go to folder 
cd /vast/projects/exasky/pascal/HACC/Seer/

# Run:
source /vast/projects/exasky/pascal/HACC/Seer/run/launchMochiServer.sh /vast/projects/exasky/pascal/HACC/Seer/run/mochi-yokan-config.json
