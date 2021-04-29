#pragma once
#include "IWorker.h"
#include "ITask.h"
#include <vector>
#include <Windows.h>

DWORD WINAPI ThreadProc(CONST LPVOID lpParam)
{
	ITask* task = (ITask*)lpParam;
	task->Execute();
	ExitThread(0);
}


class Pool
{
public:
	Pool(const std::vector<ITask*> tasks, int threadsCount)
	{
		m_threadsCount = threadsCount;
		HANDLE handle;
		for (size_t i = 0; i < tasks.size(); i++)
		{
			handle = CreateThread(NULL, i, &ThreadProc, tasks[i], CREATE_SUSPENDED, NULL);
			m_threads.push_back(CreateThread(NULL, i, &ThreadProc, tasks[i], CREATE_SUSPENDED, NULL));
		}
	}

	void AddTask(ITask* task)
	{
		HANDLE handle = CreateThread(NULL, 0, &ThreadProc, task, CREATE_SUSPENDED, NULL);
		m_threads.push_back(handle);
	}

	void Execute()
	{
		int count = 0;
		for (size_t i = 0; i < m_threads.size(); i++)
		{
			ResumeThread(m_threads[i]);
			count++;

			if (count == m_threadsCount)
			{
				WaitForMultipleObjects(i + 1, m_threads.data(), true, INFINITE);

				count = 0;
			}

		}
		WaitForMultipleObjects(m_threadsCount, m_threads.data(), true, INFINITE);
	}
private:
	int m_threadsCount;
	std::vector<HANDLE> m_threads;
};