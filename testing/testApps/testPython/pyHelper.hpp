#pragma once

#include <iostream>
#include <string>
#include <Python.h>
#include <numpy/arrayobject.h>



class CPyObject
{
  private:
	PyObject *p;

  public:
	CPyObject() : p(NULL)
	{}

	CPyObject(PyObject* _p) : p(_p)
	{}

	
	~CPyObject()
	{
		Release();
	}

	PyObject* getObject()
	{
		return p;
	}

	PyObject* setObject(PyObject* _p)
	{
		return (p=_p);
	}

	PyObject* AddRef()
	{
		if (p)
		{
			Py_INCREF(p);
		}
		return p;
	}

	void Release()
	{
		if(p)
		{
			Py_DECREF(p);
		}

		p = NULL;
	}

	PyObject* operator ->()
	{
		return p;
	}

	bool is()
	{
		return p ? true : false;
	}

	operator PyObject*()
	{
		return p;
	}

	PyObject* operator = (PyObject* pp)
	{
		p = pp;
		return p;
	}

	operator bool()
	{
		return p ? true : false;
	}
};



class CPyInstance
{

  public:
	CPyInstance()  { Py_Initialize(); }
	~CPyInstance() { Py_Finalize();   }

	CPyObject loadModule(std::string module);
	CPyObject runFunction(CPyObject module, std::string funcName);
};


inline CPyObject CPyInstance::loadModule(std::string module)
{
	CPyObject pName = PyUnicode_FromString(module.c_str());
	CPyObject pModule = PyImport_Import(pName);

	std::cout << "module " << module << std::endl;

	if (!pModule)
	{
		std::cout <<"ERROR: Module --- " << module << " not imported!" << std::endl;
		return NULL;
	}

	return pModule;
}
 

inline CPyObject CPyInstance::runFunction(CPyObject module, std::string funcName)
{
	CPyObject pFunc = PyObject_GetAttrString(module, funcName.c_str());
	if (pFunc && PyCallable_Check(pFunc))
	{
		return pFunc;
	}
	else
	{
		std::cout <<"ERROR: Function " << funcName.c_str() << "!" << std::endl;
		return NULL;
	}
}