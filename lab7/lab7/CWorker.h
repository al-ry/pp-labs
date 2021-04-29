#pragma once
#include <Windows.h>
#include "IWorker.h"



class CWorker : public IWorker
{
public:
	bool ExecuteTask(ITask* task) override;

	bool IsBusy() const override;

	static DWORD WINAPI ThreadProc(CONST LPVOID lpParam);

	~CWorker();

private:
	HANDLE m_handle;
};