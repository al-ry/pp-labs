#pragma once
#include "ITask.h"
#include <Windows.h>

class CTask : public ITask
{
public:
	CTask(int threadsCount);

	void Execute() override;

	static DWORD WINAPI ThreadProc(CONST LPVOID lpParam);

	~CTask();
private:
	int m_threadsCount;
	HANDLE* m_handle;
	int* m_params;
	bool m_IsStarted = false;
};