#include <windows.h>
#include <string>
#include <iostream>
#include <tchar.h>
#include <optional>
#include <fstream>
#include <chrono> 


using namespace std::chrono;


struct Params
{
    int i;
    std::string outFile;
    steady_clock::time_point start;
};


DWORD WINAPI ThreadProc(CONST LPVOID lpParam)
{
    auto params = (Params*)lpParam;
    std::cout << "Processing thread number: " + std::to_string(params->i) + "\n";
    std::ofstream out(params->outFile);

    for (size_t i = 0; i < 20; i++)
    {
        for (size_t i = 0; i < 100000000; i++)
        {
            int tmp = 20 * 20 * 20 + 10000000 / 20 % 20;
        }
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<milliseconds>(end - params->start);
        std::string outStr = std::to_string(duration.count()) + "\n";
        out << outStr;
    }
    ExitThread(0);
}

typedef unsigned long long ThreadsCount;

int _tmain(int argc, _TCHAR* argv[])
{
    int i;
    std::cin >> i;
    HANDLE* handle = new HANDLE[2];
    Params *p = new Params[2];
    auto start = high_resolution_clock::now();
    for (size_t i = 0; i < 2; i++)
    {
        p[i].i = i;
        p[i].outFile = "out" + std::to_string(i) + ".txt";
        p[i].start = start;
        handle[i] = CreateThread(NULL, 0, &ThreadProc, p + i, CREATE_SUSPENDED, NULL);
        ResumeThread(handle[i]);

    }
    WaitForMultipleObjects(2, handle, true, INFINITE);

    return 0;
}