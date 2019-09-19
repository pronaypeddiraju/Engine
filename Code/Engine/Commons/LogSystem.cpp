#include "Engine/Commons/LogSystem.hpp"
#include "Engine/Core/FileUtils.hpp"
#include <fstream>
#include "Engine/Core/Time.hpp"
#include <time.h>

#define WIN32_LEAN_AND_MEAN		// Always #define this before #including <windows.h>
#include <windows.h>			// #include this (massive, platform-specific) header in very few places

LogSystem* g_LogSystem = nullptr;

//------------------------------------------------------------------------------------------------------------------------------
std::string DateTime()
{
	time_t rawTime;
	struct tm * timeInfo = new struct tm;
	char buffer[80];

	time(&rawTime);
	localtime_s(timeInfo, &rawTime);

	strftime(buffer, 80, "ExecutionLog_%d-%m-%Y_%H-%M-%S", timeInfo);

	return std::string(buffer);
}

//------------------------------------------------------------------------------------------------------------------------------
void CheckCreateDirectory(const char* directory)
{
	if (CreateDirectoryA(directory, NULL) ||
		ERROR_ALREADY_EXISTS == GetLastError())
	{
		//The directory exists
	}
	else
	{
		// Failed to create directory.
		ERROR_AND_DIE("Failed to create directory for log file");
	}
}

//------------------------------------------------------------------------------------------------------------------------------
LogSystem::LogSystem(const char* fileName)
{
	m_filename = fileName;
}

//------------------------------------------------------------------------------------------------------------------------------
LogSystem::~LogSystem()
{
	//Assumes Shutdown was called
}

//------------------------------------------------------------------------------------------------------------------------------
static void LogThread()
{
	if (g_LogSystem->m_fileStream == nullptr)
	{
		// See if a file exists, if not create one
		g_LogSystem->m_fileStream = CreateFileWriteBuffer(g_LogSystem->m_filename);
		if (g_LogSystem->m_fileStream == nullptr)
		{
			ERROR_AND_DIE("Could not create log file");
			return;
		}
	}

	//Log that we have initialized
	std::string initString = "Log Initialized";
	g_LogSystem->m_fileStream->write(initString.c_str(), initString.length());

	// wait for information to write to log while running
	while (g_LogSystem->IsRunning())
	{
		// get data to write
		// potentially a dead-lock; 
		g_LogSystem->WaitForWork();

		size_t outSize;
		LogObject_T *log = (LogObject_T*)g_LogSystem->m_messages.TryLockRead(&outSize);
		while (log != nullptr)
		{
			g_LogSystem->WriteToLogFromBuffer(*log);
			g_LogSystem->RunAllHooks(log);
			g_LogSystem->m_messages.UnlockRead(log);
			log = (LogObject_T*)g_LogSystem->m_messages.TryLockRead(&outSize);
		}

		//Check for log flush
		if (g_LogSystem->m_flushRequested)
		{
			log = (LogObject_T*)g_LogSystem->m_messages.TryLockRead(&outSize);
			TODO("Get the thread to read LogObjects from buffer");
			while (log != nullptr)
			{
				g_LogSystem->WriteToLogFromBuffer(*log);
				g_LogSystem->RunAllHooks(log);
				g_LogSystem->m_messages.UnlockRead(log);
				log = (LogObject_T*)g_LogSystem->m_messages.TryLockRead(&outSize);
			}

			// flush the file
			g_LogSystem->m_fileStream->flush();
			// close the file
			g_LogSystem->m_fileStream->close();

			g_LogSystem->m_flushRequested = false;
		}

	}

	
	// flush the file
	g_LogSystem->m_fileStream->flush();
	// close the file
	g_LogSystem->m_fileStream->close();
}

//------------------------------------------------------------------------------------------------------------------------------
void LogSystem::WriteToLogFromBuffer(const LogObject_T& log)
{
	// write it
	// buffer is the c_str of the message
	std::string logWriteString("Log Entry: ");
	logWriteString += "Time: ";
	logWriteString += std::to_string(GetHPCToSeconds(log.hpcTime));
	logWriteString += "Filter: ";
	logWriteString += log.filter;
	logWriteString += "\n\t Message: ";
	logWriteString += log.line;
	if (log.callstack.m_depth != 0)
	{
		logWriteString += "\n\t Callstack: \n\t ";
		std::vector<std::string> callStackStrings = GetCallstackToString(log.callstack);
		std::vector<std::string>::iterator stringsItr = callStackStrings.begin();
		while (stringsItr != callStackStrings.end())
		{
			logWriteString += stringsItr->c_str();
			logWriteString += "\n\t ";
			++stringsItr;
		}
	}

	g_LogSystem->m_fileStream->write(logWriteString.c_str(), logWriteString.length());

	log.~LogObject_T();
}

//------------------------------------------------------------------------------------------------------------------------------
void LogSystem::LogSystemInit()
{
	CheckCreateDirectory(m_filename.c_str());

	std::string completeFilePath = m_filename;

	//Get system date and time
	std::string systemDateTime = DateTime();
	completeFilePath += systemDateTime;
	completeFilePath += ".bin";

	m_filename = completeFilePath;
	m_semaphore.Create(0, 1);
	m_messages.InitializeBuffer(2048);

	// last thing I do before returning
	m_thread = std::thread(LogThread);
}

//------------------------------------------------------------------------------------------------------------------------------
void LogSystem::LogSystemShutDown()
{
	Stop();

	DebuggerPrintf("Shutting down log system...");
	SignalWork();

	m_thread.join();
}

//------------------------------------------------------------------------------------------------------------------------------
void LogSystem::Logf(char const* filter, char const* format, ...)
{
	if (g_LogSystem == nullptr)
		return;


	bool canLog = g_LogSystem->CheckAgainstFilter(filter);

	if (canLog == false)
	{
		return;
	}

	//Do this C Sytled
	size_t logObjSize = sizeof(LogObject_T);
	
	va_list args;
	va_start(args, format);
	size_t msgLength = vsnprintf(nullptr, 0, format, args) + 1U;
	//The 1U is added to account for the null termination character

	size_t filterLength = strlen(filter) + 1U;

	size_t totalSize = logObjSize + msgLength + filterLength;
	void* buffer = g_LogSystem->m_messages.LockWrite(totalSize);

	LogObject_T* log = (LogObject_T*)buffer;
	log->hpcTime = GetCurrentTimeHPC();
	log->callstack = CallstackGet(2);
	log->callstack.m_depth = 0;
	log->line = (char*)buffer + logObjSize;
	log->filter = (char*)buffer + logObjSize + msgLength;

	vsnprintf(log->line, msgLength, format, args);
	memcpy(log->filter, filter, filterLength);
	va_end(args);

	m_messages.UnlockWrite(buffer);

	SignalWork();
}

//------------------------------------------------------------------------------------------------------------------------------
void LogSystem::LogCallstackf(char const* filter, char const* format, ...)
{
	if (g_LogSystem == nullptr)
		return;


	bool canLog = g_LogSystem->CheckAgainstFilter(filter);

	if (canLog == false)
	{
		return;
	}

	//Do this C Sytled
	size_t logObjSize = sizeof(LogObject_T);

	va_list args;
	va_start(args, format);
	size_t msgLength = vsnprintf(nullptr, 0, format, args) + 1U;
	//The 1U is added to account for the null termination character

	size_t filterLength = strlen(filter) + 1U;

	size_t totalSize = logObjSize + msgLength + filterLength;
	void* buffer = g_LogSystem->m_messages.LockWrite(totalSize);

	LogObject_T* log = (LogObject_T*)buffer;
	log->hpcTime = GetCurrentTimeHPC();
	log->callstack = CallstackGet(2);
	log->line = (char*)buffer + logObjSize;
	log->filter = (char*)buffer + logObjSize + msgLength;

	vsnprintf(log->line, msgLength, format, args);
	memcpy(log->filter, filter, filterLength);
	va_end(args);

	g_LogSystem->RunAllHooks(log);
	m_messages.UnlockWrite(buffer);

	SignalWork();
}

//------------------------------------------------------------------------------------------------------------------------------
void LogSystem::RunAllHooks(const LogObject_T* logObj)
{
	std::vector<LogHookCallback>::iterator itr;
	itr = m_logHooks.begin();

	while (itr != m_logHooks.end())
	{
		//Dereference the itr and then pass arguments to the function pointer
		//Dereferencing itr is what gives us the function pointer
		(*itr)(logObj);

		itr++;
	}
}

//------------------------------------------------------------------------------------------------------------------------------
bool LogSystem::CheckAgainstFilter(const char* filterToCheck)
{
	if (m_filterMode == true)
	{
		//we are in white list mode so we should not write this log if filter exists in set
		if (m_filterSet.find(filterToCheck) != m_filterSet.end())
		{
			//We found this filter in the set so we shouldn't write
			return false;
		}
		else
		{
			//We didn't find it in the set so this filter is writable
			return true;
		}
	}
	else
	{
		//we are in black list mode so we should write this log if filter exists in set
		if (m_filterSet.find(filterToCheck) != m_filterSet.end())
		{
			//We found the filter in the set so we should write this message to log
			return true;
		}
		else
		{
			return false;
		}
	}
}

//------------------------------------------------------------------------------------------------------------------------------
void LogSystem::LogEnableAll()
{
	m_filterMode = true;
	m_filterSet.clear();
}

//------------------------------------------------------------------------------------------------------------------------------
void LogSystem::LogDisableAll()
{
	m_filterMode = false;
	m_filterSet.clear();
}

//------------------------------------------------------------------------------------------------------------------------------
void LogSystem::LogEnable(char const* filter)
{
	if (m_filterMode == false)
	{
		//We have blacklisted things so enabling a specific filter makes sense
		m_filterSet.insert(filter);
	}
	else
	{
		return;
	}
}

//------------------------------------------------------------------------------------------------------------------------------
void LogSystem::LogDisable(char const* filter)
{
	if (m_filterMode == true)
	{
		//We have white-listed things so disabling a specific filter makes sense
		m_filterSet.insert(filter);
	}
	else
	{
		return;
	}
}

//------------------------------------------------------------------------------------------------------------------------------
void LogSystem::LogFlush()
{
	if (g_LogSystem == nullptr)
		return;

	g_LogSystem->m_flushRequested = true;
	while (g_LogSystem->m_flushRequested != false)
	{
		std::this_thread::yield();
	}

	return;
}

//------------------------------------------------------------------------------------------------------------------------------
void LogSystem::LogHook(LogHookCallback cb)
{
	m_logHooks.push_back(cb);
}

//------------------------------------------------------------------------------------------------------------------------------
void LogSystem::LogUnhook(LogHookCallback cb)
{
	std::vector<LogHookCallback>::iterator itr;
	itr = std::find(m_logHooks.begin(), m_logHooks.end(), cb);
	
	if (itr == m_logHooks.end())
	{
		return;
	}

	m_logHooks.erase(itr);
}


