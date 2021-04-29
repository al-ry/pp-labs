#include <Windows.h>
#include <string>
#include "CTask.h"
#include <iostream>


CTask::CTask(int threadsCount)
{
	m_threadsCount = threadsCount;
	m_handle = new HANDLE[m_threadsCount];
	m_params = new int[m_threadsCount];
	for (size_t i = 0; i < m_threadsCount; i++)
	{
		*m_params = i + 1;
		m_handle[i] = CreateThread(NULL, 0, &ThreadProc, m_params, CREATE_SUSPENDED, NULL);
		m_params++;
	}
}

void CTask::Execute()
{
	for (size_t i = 0; i < m_threadsCount; i++)
	{
		ResumeThread(m_handle[i]);
	}
	m_IsStarted = true;
}

DWORD __stdcall CTask::ThreadProc(const LPVOID lpParam)
{
	auto i = (int*)lpParam;
	std::cout << "Processing thread number: " + std::to_string(*i) + "\n";
	ExitThread(0);
}

CTask::~CTask()
{
	if (m_IsStarted)
	{
		WaitForMultipleObjects(m_threadsCount, m_handle, true, INFINITE);
	}
	delete[] m_handle;
}
