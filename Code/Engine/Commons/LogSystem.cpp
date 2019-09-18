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
	struct tm * timeInfo;
	char buffer[80];

	time(&rawTime);
	timeInfo = localtime(&rawTime);

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
		TODO("Get the thread to read LogObjects from buffer");
		while (log != nullptr)
		{
			//read the log object from buffer
			log = (LogObject_T*)g_LogSystem->m_messages.TryLockRead(&outSize);

			// write it
			// buffer is the c_str of the message
			std::string logWriteString("Log Entry: ");
			logWriteString += "Time: ";
			logWriteString += std::to_string(GetHPCToSeconds(log->hpcTime));
			logWriteString += "Filter: ";
			logWriteString += log->filter;
			logWriteString += "\n\t Message: ";
			logWriteString += log->line;
			logWriteString += "\n\t Callstack: \n\t ";
			std::vector<std::string> callStackStrings = GetCallstackToString(log->callstack);
			std::vector<std::string>::iterator stringsItr = callStackStrings.begin();
			while (stringsItr != callStackStrings.end())
			{
				logWriteString += stringsItr->c_str();
				logWriteString += "\n\t ";
				++stringsItr;
			}

			g_LogSystem->m_fileStream->write(logWriteString.c_str(), logWriteString.length());

			g_LogSystem->m_messages.UnlockRead(log);
		}
	}

	// flush the file
	g_LogSystem->m_fileStream->flush();
	// close the file
	g_LogSystem->m_fileStream->close();
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

	LogObject_T* log = new LogObject_T();

	log->filter = filter;

	va_list args;
	va_start(args, format);
	std::string formatString = Stringf(format);
	va_end(args);
	log->line = formatString;
	log->hpcTime = GetCurrentTimeHPC();
	log->callstack = CallstackGet(2);

	void* dataPtr = m_messages.TryLockWrite(sizeof(log));
	if (dataPtr != nullptr)
	{
		memcpy(dataPtr, log, sizeof(LogObject_T));
	}
	else
	{
		ERROR_RECOVERABLE("Couldn't write to MPSC Ring Buffer from Logf");
	}
	SignalWork();
}