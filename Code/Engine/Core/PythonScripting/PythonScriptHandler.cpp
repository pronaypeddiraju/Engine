#include "Engine/Core/PythonScripting/PythonScriptHandler.hpp"
#include "Engine/Commons/ErrorWarningAssert.hpp"
#include "Engine/Commons/EngineCommon.hpp"

//#include "Engine/ProdigyTemplateLibrary/PVectorBase.hpp"

//------------------------------------------------------------------------------------------------------------------------------
//This is where we describe our moudle functions to python by using the python function names as keys
static PyMethodDef ProdigyPyMethods[] = {
	//Python function name,			C Func to Call,			Type,				Help Message
	{	"Log",						PyHookLog,				METH_VARARGS,		"Logs using the system log"},
	{	nullptr,					nullptr,				0,					nullptr}	//Terminating element
};

//------------------------------------------------------------------------------------------------------------------------------
//This is where we will define the module for Python (A.K.A Module defenition)
static PyModuleDef ProdigyModule = {
	// base,					name,			documentation,    size,		methods,			slots,   tarverse_cb,   clear_cb,   free_cb
	PyModuleDef_HEAD_INIT,		"ProdigyEngine",		nullptr,          -1,       ProdigyPyMethods,   nullptr, nullptr,       nullptr,    nullptr
};

//------------------------------------------------------------------------------------------------------------------------------
//Now we will create our actual module since it is defined above
STATIC PyObject* ProdigyModuleInit()
{
	return PyModule_Create(&ProdigyModule);
}

//------------------------------------------------------------------------------------------------------------------------------
void PythonStartup()
{
	wchar_t const* program = L"TestProgram";

	// presetup
	Py_SetProgramName(program);
	Py_SetPath(L"./CPython_3.7Lib.zip; .//Data//Scripts//; ");

	// Add in any modules you want with their associated create function 
	PyImport_AppendInittab("ProdigyEngine", &ProdigyModuleInit);

	// initialize the interpreter
	Py_Initialize();

	
	// do things?
	PyRun_SimpleString(
		"from time import time,ctime\n"
		"print('Today is', ctime(time()) )\n"
	);

	//Run the logic to import our module
	PyRun_SimpleString(
		"import sys\n"
		"import ProdigyEngine\n"
		"class LogOutput:\n"
		"	def write(self, txt):\n"
		"		ProdigyEngine.Log(txt)\n"
		"	def flush(self):\n"
		"		pass\n"
		"logOutput = LogOutput()\n"
		"sys.stdout = logOutput\n"
		"sys.stderr = logOutput\n"
		"ProdigyEngine.Log('Prodigy Log Test')\n"
	);

	PyRun_SimpleString(
		"print(234)\n"
		"ProdigyEngine.Log(123)\n"
	);

}

//------------------------------------------------------------------------------------------------------------------------------
void PythonShutdown()
{
	// cleanup the system
	Py_Finalize();
}

//------------------------------------------------------------------------------------------------------------------------------
PyObject* PyHookLog(PyObject* self, PyObject* args)
{
	UNUSED(self);

	DebuggerPrintf("Python Log: ");

	int arg_count = (int)PyTuple_Size(args);
	if (arg_count > 0) {
		PyObject* arg0 = PyTuple_GetItem(args, 0);

		PyObject* str = nullptr;
		if (PyUnicode_CheckExact(arg0)) {
			str = PyUnicode_AsEncodedString(arg0, "utf-8", "~E~");
		}
		else {
			PyObject* repr = PyObject_Repr(arg0);
			str = PyUnicode_AsEncodedString(repr, "utf-8", "~E~");
			Py_DecRef(repr);
		}

		char const* bytes = PyBytes_AS_STRING(str);
		DebuggerPrintf(bytes);
		
		DebuggerPrintf("\n");
		
		Py_DecRef(str);
	}

	Py_INCREF(Py_None);
	return Py_None;
}
