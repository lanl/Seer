module purge
module load openmpi/3.1.0-gcc_7.3.0
module load papi/5.6.1

spack env activate scidac
spack load -r margo
spack load -r sdskeyval
spack load -r py-sdskv
spack load vtk
spack load cmake
spack load /wnmdss5	# paraview@5.7.0
spack load /pse7k4b # osmesa