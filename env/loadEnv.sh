module load openmpi/3.1.6-gcc_9.4.0
module load cmake
module list

spack load /hiu7yh7 #mochi-yokan@0.4.2 
spack load conduit
spack load py-blosc2
spack load py-notebook
#spack load py-pandas
spack load /zxsfkwb #numpy
spack load py-matplotlib
spack load /rkb63dt #blosc 2
spack load /axdmuus #nholmann-json

export LD_LIBRARY_PATH=/vast/home/pascalgrosset/spack/opt/spack/linux-rhel8-haswell/gcc-9.4.0/c-blosc2-2.11.1-rkb63dt4yt5xzwamtpdhixbvbmgbwskm/lib64/:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH=/vast/home/pascalgrosset/spack/opt/spack/linux-rhel8-haswell/gcc-9.4.0/mochi-yokan-0.4.2-hiu7yh7om6nmyc2ahuknpdsov5k64zcj/lib/:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH=/vast/home/pascalgrosset/spack/opt/spack/linux-rhel8-haswell/gcc-9.4.0/mochi-margo-0.15.0-gbcextlqglkmsymdjchg7pornsleer6s/lib/:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH=/projects/insituperf/SZ3/install/lib64/:$LD_LIBRARY_PATH
