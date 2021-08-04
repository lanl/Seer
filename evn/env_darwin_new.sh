module load cmake
spack load -r mochi-margo
spack load -r mochi-sdskv+leveldb
spack load -r py-mochi-sdskv
spack load -r papi

export PATH=/projects/insituperf/tau2/x86_64/bin:$PATH
export TAU_PLUGINS_PATH=/projects/insituperf/tau2/x86_64/lib/shared-papi-mpi