#include "ITask.h"
#include "CWorker.h"
#include <iostream>
#include <sstream>


bool Pool::ExecuteTask(ITask* task)
{
	if (IsBusy())
	{
		std::stringstream message;
		message << "Task with address: " << task << " cannot be executed\n";
		std::cout << message.str();
		return false;
	}

	m_handle = CreateThread(NULL, 0, &ThreadProc, task, 0, NULL);

	return true;
}

bool Pool::IsBusy() const
{
	DWORD statusCode;
	GetExitCodeThread(m_handle, &statusCode);
	if (statusCode == STILL_ACTIVE)
	{
		return true;
	}
	return false;
}

DWORD __stdcall Pool::ThreadProc(const LPVOID lpParam)
{
	ITask* task = (ITask*)lpParam;
	task->Execute();
	ExitThread(0);
}


Pool::~Pool()
{
	WaitForSingleObject(m_handle, INFINITE);
}
