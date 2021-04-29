#pragma once
#include "List.h"
#include "LogWriter.h"
#include <windows.h>
#include <string>
#include <stdexcept>

class LogBuffer
{
public:
	LogBuffer()
	{
		if (!InitializeCriticalSectionAndSpinCount(&CriticalSection,
			0x00000400))
		{
			throw std::exception("Cant initialize critical section.\n");
		}
		event = CreateEvent(nullptr, TRUE, FALSE, TEXT("LogEvent"));
		thread = CreateThread(NULL, 0, &LogSizeMonitoringThread, this, 0, NULL);
	}
	~LogBuffer()
	{
		DeleteCriticalSection(&CriticalSection);
	}
	void LogInfo(const std::string& log)
	{
		EnterCriticalSection(&CriticalSection);

		if (logList.GetSize() == BUF_MAX_SIZE)
		{
			LogAllInfoIntoFile();
		}
		logList.AppendBack(log);

		LeaveCriticalSection(&CriticalSection);
	}


private:
	static DWORD WINAPI LogSizeMonitoringThread(CONST LPVOID lpParam)
	{

		auto param = (LogBuffer*)lpParam;
		DWORD dwWaitResult = WaitForSingleObject(param->event, INFINITE);
		if (dwWaitResult == WAIT_OBJECT_0)
		{
			param->writer.Log(param->logList);
		}
		ExitThread(0);
	}

	void LogAllInfoIntoFile()
	{
		SetEvent(event);

		DWORD dwWaitResult = WaitForSingleObject(thread, INFINITE);

		if (dwWaitResult == WAIT_OBJECT_0)
		{
			ResetEvent(event);
			thread = CreateThread(NULL, 0, &LogSizeMonitoringThread, this, 0, NULL);
		}	
	}
private:
	CRITICAL_SECTION CriticalSection;
	LogWriter writer;
	const int BUF_MAX_SIZE = 1024;
	CMyList<std::string> logList;
	HANDLE event;
	HANDLE thread;
};


