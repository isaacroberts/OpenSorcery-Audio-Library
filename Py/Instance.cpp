//
//  Instance.cpp
//  
//
//  Created by Isaac Roberts on 9/15/22.
//

// Created by Isaac Roberts



Instance::Instance()
{
	Py_Initialize();
	
	//Run a simple string
	PyRun_SimpleString("from time import time,ctime\n"
	"print('Today is',ctime(time()))\n");


}
Instance::Instance(const Instance& other)
{
	
//	PyObject *pName, *pModule, *pFunc, *pArgs, *pValue;
//	pName = PyUnicode_FromString((char*)"script");
//	pModule = PyImport_Import(pName);
//	pFunc = PyObject_GetAttrString(pModule, (char*)"test");
//	pArgs = PyTuple_Pack(1, PyUnicode_FromString((char*)"Greg"));
//	pValue = PyObject_CallObject(pFunc, pArgs);
//	auto result = _PyUnicode_AsString(pValue);
//	std::cout << result << std::endl;

}
Instance& Instance::operator=(const Instance& other)
{
	
	return *this;
}


//Destructor
Instance::~Instance()
{
	Py_Finalize();

}

