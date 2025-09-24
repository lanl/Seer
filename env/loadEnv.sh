module load openmpi/3.1.6-gcc_9.4.0
module load cmake/3.29.2

source /vast/home/pascalgrosset/loadSpack.sh
spack env activate vevn_spack_seerx

# Load the spack modules
spack load python
spack load mochi-yokan
spack load c-blosc2
spack load nlohmann-json

source /vast/projects/exasky/pascal/HACC/Seer/venv_seerx/bin/activate

export LD_LIBRARY_PATH=/vast/home/pascalgrosset/software/spack/opt/spack/linux-broadwell/c-blosc2-2.15.1-fbe56n4kdqfglsxcsdtzpxbiejhbcysh/lib64/:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH=/vast/home/pascalgrosset/software/spack/opt/spack/linux-broadwell/mochi-yokan-0.8.1-njadzndq6fkjnrva3utxazt4psj6pc2v/lib/:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH=/vast/home/pascalgrosset/software/spack/opt/spack/linux-broadwell/mochi-margo-0.19.1-e36frcvduknklrl3byh2bwogtulbuoe6/lib/:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH=/vast/projects/exasky/pascal/HACC/SZ3/install/lib64/:$LD_LIBRARY_PATH


# Old
# module load openmpi/3.1.6-gcc_9.4.0
# module load cmake
# module list


# source /vast/home/pascalgrosset/loadSpack.sh

# spack load /hiu7yh7 #mochi-yokan@0.4.2 
# spack load conduit
# spack load py-blosc2
# spack load py-notebook
# #spack load py-pandas
# spack load /zxsfkwb #numpy
# spack load py-matplotlib
# spack load /rkb63dt #blosc 2
# spack load /axdmuus #nholmann-json

# export LD_LIBRARY_PATH=/vast/home/pascalgrosset/spack/opt/spack/linux-rhel8-haswell/gcc-9.4.0/c-blosc2-2.11.1-rkb63dt4yt5xzwamtpdhixbvbmgbwskm/lib64/:$LD_LIBRARY_PATH
# export LD_LIBRARY_PATH=/vast/home/pascalgrosset/spack/opt/spack/linux-rhel8-haswell/gcc-9.4.0/mochi-yokan-0.4.2-hiu7yh7om6nmyc2ahuknpdsov5k64zcj/lib/:$LD_LIBRARY_PATH
# export LD_LIBRARY_PATH=/vast/home/pascalgrosset/spack/opt/spack/linux-rhel8-haswell/gcc-9.4.0/mochi-margo-0.15.0-gbcextlqglkmsymdjchg7pornsleer6s/lib/:$LD_LIBRARY_PATH
# export LD_LIBRARY_PATH=/projects/insituperf/SZ3/install/lib64/:$LD_LIBRARY_PATH
