#include <windows.h>
#include <string>
#include <iostream>
#include <ctime>


CRITICAL_SECTION CriticalSection;


DWORD WINAPI ThreadProc(CONST LPVOID lpParam)
{
	//EnterCriticalSection(&CriticalSection);

	int* working_variable = (int*)lpParam;
	
	int j = *working_variable;
	std::srand(std::time(nullptr));
	int delta = std::rand() % 10;
	j += delta;
	*working_variable = j;


	for (size_t i = 0; i < 1000000; i++);


	if (j != *working_variable)
	{
		std::cout << "Error\n ";
	}
	//LeaveCriticalSection(&CriticalSection);

	ExitThread(0);
}


int main(int argc)
{

	int i;
	std::cin >> i;
	int working_variable = 1;

	if (!InitializeCriticalSectionAndSpinCount(&CriticalSection,
		0x00000400))
		return 0;

	HANDLE* handles = new HANDLE[2];

	for (int i = 0; i < 2; i++) {
		handles[i] = CreateThread(NULL, i, &ThreadProc, &working_variable, CREATE_SUSPENDED, NULL);
	}

	for (int i = 0; i < 2; i++) {
		ResumeThread(handles[i]);
	}

	WaitForMultipleObjects(2, handles, true, INFINITE);


	DeleteCriticalSection(&CriticalSection);

	return 0;
}

