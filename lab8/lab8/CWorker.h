#pragma once
#include <Windows.h>
#include "IWorker.h"



class Pool : public IWorker
{
public:
	bool ExecuteTask(ITask* task) override;

	bool IsBusy() const override;

	static DWORD WINAPI ThreadProc(CONST LPVOID lpParam);
	~Pool();

private:
	HANDLE m_handle;
};
