module purge
module load openmpi/3.1.0-gcc_7.3.0
module load papi/5.6.1

spack load -r /ytwvrre # margo
spack load -r /xvkjxkz #sdskeyval
spack load -r /4m7jqu6 #py-sdskv
spack load -r /tpy5jxl #py-margo
#spack load /zioiaxa #vtk
spack load /3k2gxxp #cmake
spack load /wnmdss5	# paraview@5.7.0
spack load /pse7k4b # osmesa

export LD_LIBRARY_PATH=/home/pascalgrosset/spack/opt/spack/linux-centos7-ivybridge/gcc-7.3.0/paraview-5.7.0-wnmdss5zmahq2pt53aai5gcu2kswkxif/lib64:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH=/projects/opt/centos7/papi/5.6.1/lib/:$LD_LIBRARY_PATH

echo "PAPI inc /projects/opt/centos7/papi/5.6.1/include"
echo "PAPI lib /projects/opt/centos7/papi/5.6.1/lib/libpapi.so"