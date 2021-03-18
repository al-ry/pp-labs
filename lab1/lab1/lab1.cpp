
#include <windows.h>
#include <string>
#include <iostream>
#include <tchar.h>
#include <optional>


DWORD WINAPI ThreadProc(CONST LPVOID lpParam)
{
    auto i = (int *)lpParam;
    std::cout <<  "Processing thread number: " + std::to_string(*i) + "\n";
    ExitThread(0);
}

typedef unsigned long long ThreadsCount;

std::optional<ThreadsCount> ParseArgs(int argsCount, _TCHAR** args)
{
    if (argsCount == 2)
    {
        try
        {
            return std::stoull(args[1]);
        }
        catch (const std::exception&)
        {
            return std::nullopt;
        }
    }
    else
    {
        return std::nullopt;
    }
}


int _tmain(int argc, _TCHAR* argv[])
{
    auto threadsCountOpt = ParseArgs(argc, argv);

    if (!threadsCountOpt)
    {
        return 1;
    }
    ThreadsCount threadsCount = *threadsCountOpt;
    HANDLE* handle = (HANDLE*)malloc(sizeof(HANDLE*) * threadsCount);
    int* number = (int*)malloc(sizeof(int) * threadsCount);
    for (size_t i = 0; i < threadsCount; i++)
    {
        *number = i + 1;
        number++;
        handle[i] = CreateThread(NULL, 0, &ThreadProc, number, CREATE_SUSPENDED, NULL);
        ResumeThread(handle[i]);
    }
    WaitForMultipleObjects(threadsCount, handle, true, INFINITE);

    for (size_t i = 0; i < threadsCount; i++)
    {
        free(&number[i]);
    }
    return 0;
}
