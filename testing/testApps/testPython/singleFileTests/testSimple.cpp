#include <Python.h>

int
main(int argc, char *argv[])
{
  //Py_SetProgramName(argv[0]);  /* optional but recommended */
  Py_Initialize();
  PyRun_SimpleString("from time import time,ctime\n"
                     "print('Today is',ctime(time())\n)");
  Py_Finalize();
  return 0;
}

// g++ testSimple.cpp -I/home/pascal/software/spack/opt/spack/linux-linuxmint19-broadwell/gcc-7.4.0/python-3.7.6-upbvnw2r6tdy3bfqgctm2ik2uwdmcpef/include/python3.7m -L/home/pascal/software/spack/opt/spack/linux-linuxmint19-broadwell/gcc-7.4.0/python-3.7.6-upbvnw2r6tdy3bfqgctm2ik2uwdmcpef/lib -lpython3.7m