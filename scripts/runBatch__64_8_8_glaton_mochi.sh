#!/bin/bash
#SBATCH -N 8
#SBATCH --ntasks-per-node 8
#SBATCH -p galton

export MPIP="-t 10.0"
projectpath=/projects/insituperf/HACC_Insitu/
pythonExe=/home/pascalgrosset/spack/opt/spack/linux-centos7-ivybridge/gcc-7.3.0/python-3.7.4-sudj2bfj657htfjbjzldee6dhinrtd6e/bin/python

# load modules
export SPACK_ROOT=$HOME/spack
source $SPACK_ROOT/share/spack/setup-env.sh

#source $projectpath/trunk/nbody/simulation/InWrap/scripts/env_setup_darwin.sh
module load openmpi/3.1.0-gcc_7.3.0
module load papi/5.6.1
module load cmake-3.15.4-gcc-7.3.0-3k2gxxp

module load paraview-5.7.0-gcc-7.3.0-inofee5
module load mesa-18.3.6-gcc-7.3.0-pse7k4b

module load py-sdskv-0.1.3-gcc-7.3.0-4m7jqu6
module load py-margo-0.2.4-gcc-7.3.0-tpy5jxl
module load py-numpy-1.17.3-gcc-7.3.0-ykvsnhh
module load openblas-0.3.7-gcc-7.3.0-kmof7rh
module load python-3.7.4-gcc-7.3.0-sudj2bf
module load expat-2.2.9-gcc-7.3.0-zjydyfm
module load libbsd-0.9.1-gcc-7.3.0-mgeakk3
module load gettext-0.20.1-gcc-7.3.0-gmg5emj
module load libxml2-2.9.9-gcc-7.3.0-dmzukqo 
module load libiconv-1.16-gcc-7.3.0-xgfrzs2
module load xz-5.2.4-gcc-7.3.0-du6t2kz
module load tar-1.32-gcc-7.3.0-r5caeax
module load libffi-3.2.1-gcc-7.3.0-f3ichtg
module load openssl-1.1.1d-gcc-7.3.0-fy57jvx
module load sqlite-3.30.1-gcc-7.3.0-kpbesi4

module load margo-0.5.2-gcc-7.3.0-ytwvrre
module load argobots-develop-gcc-7.3.0-567rfnn 
module load mercury-master-gcc-7.3.0-2vymgd3
module load boost-1.70.0-gcc-7.3.0-ijtf752
module load bzip2-1.0.8-gcc-7.3.0-f3qm5nl
module load zlib-1.2.11-gcc-7.3.0-g5dps7r
module load libfabric-1.8.1-gcc-7.3.0-wavowuy

module load sdskeyval-0.1.6-gcc-7.3.0-xvkjxkz
module load abt-io-0.3.1-gcc-7.3.0-z3ep2v5
module load autoconf-2.69-gcc-7.3.0-ejhbb46
module load m4-1.4.18-gcc-7.3.0-orpl3q4
module load libsigsegv-2.12-gcc-7.3.0-ktntzz2
module load perl-5.30.0-gcc-7.3.0-f334ryv
module load gdbm-1.18.1-gcc-7.3.0-gdfz4n5
module load readline-8.0-gcc-7.3.0-zdyv3xt
module load ncurses-6.1-gcc-7.3.0-dcwnba3
module load openssl-1.1.1d-gcc-7.3.0-fy57jvx
module load pkg-config-0.29.2-gcc-7.3.0-vvuzr6u
module load automake-1.16.1-gcc-7.3.0-64fsg7q
module load berkeley-db-6.2.32-gcc-7.3.0-s6nft5w
module load leveldb-1.22-gcc-7.3.0-5txgfel
module load snappy-1.1.7-gcc-7.3.0-v3exnbf

module list



# go to folder 
cd $projectpath/run/


# Get IP
nodes=$(scontrol show hostnames $SLURM_JOB_NODELIST) # Getting the node names
nodes_array=( $nodes )
node1=${nodes_array[0]}

ip=$(srun --nodes=1 --ntasks=1 -w $node1 hostname --ip-address)
echo $ip

# Run:
# Modify input script with ip
mpirun $pythonExe $projectpath/trunk/nbody/simulation/InWrap/python-utils/createJson.py $ip $projectpath/trunk/nbody/simulation/InWrap/inputs/input-HACC.json

echo "create Json done"

# Execute jobs in parallel: mopchi server + App
mpirun $pythonExe $projectpath/trunk/nbody/simulation/InWrap/python-utils/launchServer.py $projectpath/trunk/nbody/simulation/InWrap/inputs/input-HACC.json &
mpirun $projectpath/trunk/Darwin/mpi/bin/hacc_pm -n inputs/indat.params -insitu /projects/insituperf/HACC/trunk/nbody/simulation/InWrap/inputs/input-HACC.json -f /projects/insituperf/HACC/run/mpitrace

