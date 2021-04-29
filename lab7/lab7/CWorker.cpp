#include "CWorker.h"
#include "CTask.h"
#include <iostream>
#include <sstream>

bool CWorker::ExecuteTask(ITask* task)
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

bool CWorker::IsBusy() const
{
	DWORD statusCode;
	GetExitCodeThread(m_handle, &statusCode);
	if (statusCode == STILL_ACTIVE)
	{
		return true;
	}
	return false;
}

DWORD __stdcall CWorker::ThreadProc(const LPVOID lpParam)
{
	ITask* task = (ITask*)lpParam;
	task->Execute();
	ExitThread(0);
}


CWorker::~CWorker()
{
	WaitForSingleObject(m_handle, INFINITE);
}
