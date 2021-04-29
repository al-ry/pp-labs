#include "bmp.h"
#include "List.h"
#include <cassert>
#include "LogBuffer.h"

struct Params
{
    bitmap* bmp;
    uint32_t startW;
    uint32_t endW;
    uint32_t startH;
    uint32_t endH;
    LogBuffer *logBuf;
    steady_clock::time_point start;
};

//http://blog.ivank.net/fastest-gaussian-blur.html

void blur_with_threads(int radius, Params* p)
{
    float rs = ceil(radius * 2.57);
    for (int i = p->startH; i < p->endH; ++i)
    {
        for (int j = p->startW; j < p->endW; ++j)
        {
            double r = 0, g = 0, b = 0;
            double count = 0;

            for (int iy = i - rs; iy < i + rs + 1; ++iy)
            {
                for (int ix = j - rs; ix < j + rs + 1; ++ix)
                {
                    auto x = std::min(static_cast<int>(p->endW) - 1, std::max(0, ix));
                    auto y = std::min(static_cast<int>(p->endH) - 1, std::max(0, iy));

                    auto dsq = ((ix - j) * (ix - j)) + ((iy - i) * (iy - i));
                    auto wght = std::exp(-dsq / (2.0 * radius * radius)) / (M_PI * 2.0 * radius * radius);

                    rgb32* pixel = p->bmp->getPixel(x, y);

                    r += pixel->r * wght;
                    g += pixel->g * wght;
                    b += pixel->b * wght;
                    count += wght;
                }
            }

            rgb32* pixel = p->bmp->getPixel(j, i);
            pixel->r = std::round(r / count);
            pixel->g = std::round(g / count);
            pixel->b = std::round(b / count);

            auto end = high_resolution_clock::now();
            auto duration = duration_cast<milliseconds>(end - p->start);
            std::string outStr = std::to_string(duration.count()) + "\n";
            p->logBuf->LogInfo(outStr);
        }
    }
}

DWORD WINAPI ThreadProc(CONST LPVOID lpParam)
{
    auto params = static_cast<Params*>(lpParam);
    blur_with_threads(3, params);
    ExitThread(0);
}

void ProccesImageBlurWithThreads(bitmap* bmp, int threadsCount, int coreCount, steady_clock::time_point& start, int* threadsPriority)
{
    unsigned int lineW = bmp->getWidth() / threadsCount;
    unsigned int lineH = bmp->getHeight() / threadsCount;
    unsigned int remainingPixels = bmp->getWidth() % threadsCount;
    Params* prm = new Params[threadsCount];
    LogBuffer* logBuf = new LogBuffer();
    for (size_t i = 0; i < threadsCount; i++)
    {
        prm[i].bmp = bmp;
        prm[i].startH = 0;
        prm[i].endH = bmp->getHeight();
        prm[i].startW = lineW * i;
        prm[i].start = start;
        prm[i].logBuf = logBuf;
        if (i == threadsCount - 1)
        {
            prm[i].endW = lineW * (i + 1) + remainingPixels;
        }
        else
        {
            prm[i].endW = lineW * (i + 1);
        }
    }


    HANDLE* handles = new HANDLE[threadsCount];
    for (int i = 0; i < threadsCount; i++)
    {
        handles[i] = CreateThread(NULL, i, &ThreadProc, &prm[i], CREATE_SUSPENDED, NULL);
        SetThreadPriority(handles[i], threadsPriority[i]);
        SetThreadAffinityMask(handles[i], (1 << coreCount) - 1);
    }

    for (int i = 0; i < threadsCount; i++) {
        ResumeThread(handles[i]);
    }

    WaitForMultipleObjects(threadsCount, handles, true, INFINITE);

    delete[] prm;
    delete[] handles;
}

void SetThreadsPriority(int* pArr, int p1, int p2, int p3)
{
    pArr[0] = THREAD_PRIORITY_ABOVE_NORMAL;
    pArr[1] = THREAD_PRIORITY_NORMAL;
    pArr[2] = THREAD_PRIORITY_BELOW_NORMAL;
}

int main(int argc, const char* argv[])
{
    auto start = high_resolution_clock::now();
    bitmap bmp{ argv[1] };
    int threadsCount = std::stoi(argv[3]);
    int coreCount = std::stoi(argv[4]);
    int* threadsPriority = new int[threadsCount];

    SetThreadsPriority(threadsPriority, 1, 1, 1);
    ProccesImageBlurWithThreads(&bmp, threadsCount, coreCount, start, threadsPriority);
    bmp.save(argv[2]);
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stop - start);
    std::cout << duration.count() << std::endl;

    delete[] threadsPriority;

    return 0;
}