#include <stdio.h>
#include <Python.h>
#include <numpy/arrayobject.h>
#include "pyHelper.hpp"


int main(int argc, char *argv[])
{
	if (argc < 3) {
        fprintf(stderr,"Usage: call pythonfile funcname [args]\n");
        return 1;
    }

	Py_Initialize();

	CPyInstance hInstance;
	
	PyRun_SimpleString("import sys");
	PyRun_SimpleString("sys.path.append(\"../modules\")");

	PyErr_Print();
	CPyObject pName = PyUnicode_FromString(argv[1]);
	CPyObject pModule = PyImport_Import(pName);

	int dims[1];
	dims[0] = 4;
	float *array = new float[dims[0]];
	for (int i=0; i<dims[0]; i++)
		array[i] = i*5;

	if (pModule)
	{
		CPyObject pFunc = PyObject_GetAttrString(pModule, argv[2]);
		if (pFunc && PyCallable_Check(pFunc))
		{
			// PyObject *pArgs, *pValue1, *pValue2;
			// pArgs = PyTuple_New(2);
			// pValue1 = PyLong_FromLong(6);
			// pValue2 = PyLong_FromLong(7);
			// PyTuple_SetItem(pArgs, 0, pValue1);
			// PyTuple_SetItem(pArgs, 1, pValue2);

			// CPyObject pValue = PyObject_CallObject(pFunc, pArgs);
			// printf("product = %ld\n", PyLong_AsLong(pValue));

			import_array();

			PyObject *pArgs, *arr;
			npy_intp dim[1];
			dim[0] = 4;
			//arr = PyArray_SimpleNewFromData(1, &dim[0], NPY_FLOAT32, reinterpret_cast<void*>(array));
			arr = PyArray_SimpleNewFromData(1, &dim[0], NPY_FLOAT32, array);
			//arr = PyArray_SimpleNewFromData(1, dim, NPY_FLOAT64, &array);

			pArgs = PyTuple_New(1);
    		PyTuple_SetItem(pArgs,0, arr);

    		CPyObject pValue = PyObject_CallObject(pFunc, pArgs);
			
		}
		else
		{
			std::cout << "ERROR: function getInteger()" << std::endl;
		}
	}
	else
	{
		PyErr_Print();
		std::cout << "ERROR: Module vvvv not imported"<< std::endl;
	}

	return 0;
}

// Example run:
// ./testPy multiply testArray