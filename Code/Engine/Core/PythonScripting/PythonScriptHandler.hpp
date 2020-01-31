#pragma once
#include "ThirdParty/CPython_3.7/Python.h"

//------------------------------------------------------------------------------------------------------------------------------
void PythonStartup();
void PythonShutdown();

//------------------------------------------------------------------------------------------------------------------------------
PyObject* PyHookLog(PyObject* self, PyObject* args);

static PyObject* ProdigyModuleInit();