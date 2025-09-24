#!/bin/bash
#SBATCH -N 8
#SBATCH --ntasks-per-node 1
#SBATCH -p scaling

source /vast/home/pascalgrosset/loadSpack.sh
#spack env activate vevn_spack_seerx_test2
spack env activate vevn_spack_seerx
spack load mochi-yokan


# go to folder 
cd /vast/projects/exasky/pascal/HACC/Seer/simple_app

# Run:
#mpirun /vast/projects/exasky/pascal/HACC/Seer/simple_app/test_yokan2 192.168.81.79:39607
mpirun /vast/projects/exasky/pascal/HACC/Seer/simple_app/sim_test /vast/projects/exasky/pascal/HACC/Seer/simple_app/mochi-yokan-config.json