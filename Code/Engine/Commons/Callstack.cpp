#include "Engine/Commons/Callstack.hpp"
#include "Engine/Commons/ErrorWarningAssert.hpp"

#define WIN32_LEAN_AND_MEAN		// Always #define this before #including <windows.h>
#include <windows.h>			// #include this (massive, platform-specific) header in very few places
#include <DbgHelp.h>

#if ( defined( _WIN64 ) & defined( _DEBUG ) )
#pragma comment( lib, "ThirdParty/WinDbg/dbghelp.lib" )
#endif

//------------------------------------------------------------------------------------------------------------------------------
Callstack CallstackGet(uint skip_frames /*= 0*/)
{
	Callstack stackTraceObject;
	stackTraceObject.m_depth = CaptureStackBackTrace(skip_frames, MAX_TRACE, stackTraceObject.m_trace, (PDWORD)stackTraceObject.m_hash);

	return stackTraceObject;
}

//------------------------------------------------------------------------------------------------------------------------------
std::vector<std::string> CallstackToString(Callstack const& callStack)
{
	HANDLE pHandle = GetCurrentProcess();

	SymInitialize(pHandle, nullptr, true);
	std::vector<std::string> callStackStrings;
	
	PIMAGEHLP_SYMBOL64 symbol = (PIMAGEHLP_SYMBOL64)malloc(sizeof(PIMAGEHLP_SYMBOL64) + (TRACE_MAX_FUNCTION_NAME_LENGTH - 1) * sizeof(TCHAR));
	memset(symbol, 0, sizeof(PIMAGEHLP_SYMBOL64) + (TRACE_MAX_FUNCTION_NAME_LENGTH - 1) * sizeof(TCHAR));
	symbol->MaxNameLength = TRACE_MAX_FUNCTION_NAME_LENGTH;
	symbol->SizeOfStruct = sizeof(PIMAGEHLP_SYMBOL64);
	PDWORD64 displacement = 0;
	IMAGEHLP_LINE64 *line = (IMAGEHLP_LINE64 *)malloc(sizeof(IMAGEHLP_LINE64));
	memset(line, 0, sizeof(IMAGEHLP_LINE64));
	line->SizeOfStruct = sizeof(IMAGEHLP_LINE64);

	for (uint stackTraceIndex = 0; stackTraceIndex < callStack.m_depth - 2; ++stackTraceIndex)
	{
		bool symResult = SymGetSymFromAddr64(pHandle, (DWORD64)callStack.m_trace[stackTraceIndex], displacement, (PIMAGEHLP_SYMBOL64)symbol);
		if (symResult)
		{
			DWORD pdwDisplacement = 0;
			bool lineResult = SymGetLineFromAddr64(pHandle, (DWORD64)callStack.m_trace[stackTraceIndex], &pdwDisplacement, line);
			if (lineResult)
			{
				//DebuggerPrintf("CallStack Function at index %d is %s in %s, address 0x%0X\n", stackTraceIndex, symbol->Name, line->FileName, (uint)symbol->Address);
				DebuggerPrintf("%s(%d):%s \n", line->FileName, line->LineNumber, symbol->Name);
				callStackStrings.push_back(line->FileName);
			}
			else
			{
				DebuggerPrintf("Error from SymGetSymFromAddr64: %lu.\n", GetLastError());
			}
		}
		else
		{
			DebuggerPrintf("Error from SymFromAddr: %lu.\n", GetLastError());
		}
	}

	return callStackStrings;
}
