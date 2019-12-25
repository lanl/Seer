
#include <iostream>
#include <vector>
#include "pyHelper.hpp"

int main()
{
    CPyInstance pyInstance;


	//PyRun_SimpleString("hello.py");
    


	// CPyObject pName = PyUnicode_FromString("hello");
	// CPyObject pModule = PyImport_Import(pName);

	// if (pModule)
	// {
	// 	CPyObject pFunc = PyObject_GetAttrString(pModule, "sum");
	// 	if (pFunc && PyCallable_Check(pFunc))
	// 	{
	// 		int array[2] = {7, 6};
	// 		int numArgs = 2;

	// 		CPyObject pArgs = PyTuple_New(numArgs);
	// 		for (int i=0; i<numArgs; i++)
	// 		{
    //         	PyTuple_SetItem(pArgs, i, PyLong_FromLong(array[i]));
	// 		}

	// 		CPyObject pValue = PyObject_CallObject(pFunc, pArgs);
	// 		std::cout << "sum = " <<  PyLong_AsLong(pValue) << std::endl;
	// 	}
	// 	else
	// 	{
	// 		std::cout <<"ERROR: function getInteger()\n" << std::endl;
	// 	}
	// }
	// else
	// {
	// 	std::cout <<"ERROR: Module not imported\n" << std::endl;
	// }



	CPyObject pModule = pyInstance.loadModule("hello");
	if (pModule)
	{
		CPyObject pFunc = pyInstance.runFunction(pModule, "sum");
		if (pFunc)
		{
			int array[2] = {7, 6};

			std::vector<float> vec;
			vec.push_back(1);
			vec.push_back(2);
			vec.push_back(3);
			vec.push_back(4);

			int numArgs = 2;

			CPyObject pArgs = PyTuple_New(3);


			//CPyObject pxVec = PyTuple_New(vec.size());
			//for (int i=0; i<vec.size(); i++)
            //	PyTuple_SetItem(pxVec, i, PyFloat_FromDouble(vec[i]) );

			CPyObject pxVec = PyArray_SimpleNewFromData(1, 4, NPY_FLOAT16, &vec[0])

			PyTuple_SetItem(pArgs, 0, PyLong_FromLong(array[0]));
			PyTuple_SetItem(pArgs, 1, pxVec);


			PyTuple_SetItem(pArgs, 2, PyLong_FromLong(vec.size()));

			CPyObject pValue = PyObject_CallObject(pFunc, pArgs);
	 		std::cout << "sum = " <<  PyLong_AsLong(pValue) << std::endl;
		}
	}


	return 0;
}

// export PYTHONPATH=/home/pascal/projects/InWrap/testing/testApps/testPython/:$PYTHONPATH