#include <algorithm>
#include <iostream>
#include <fstream>
#include <string>
#define NOMINMAX
#define _USE_MATH_DEFINES
#include <math.h>
#include <chrono> 

#if defined(_WIN32) || defined(_WIN64)

#include <windows.h>
#endif

using namespace std::chrono;

typedef struct
{
    uint8_t r, g, b, a;
} rgb32;


#if !defined(_WIN32) && !defined(_WIN64)
#pragma pack(2)
typedef struct
{
    uint16_t bfType;
    uint32_t bfSize;
    uint16_t bfReserved1;
    uint16_t bfReserved2;
    uint32_t bfOffBits;
} BITMAPFILEHEADER;
#pragma pack()


#pragma pack(2)
typedef struct
{
    uint32_t biSize;
    int32_t biWidth;
    int32_t biHeight;
    uint16_t biPlanes;
    uint16_t biBitCount;
    uint32_t biCompression;
    uint32_t biSizeImage;
    int16_t biXPelsPerMeter;
    int16_t biYPelsPerMeter;
    uint32_t biClrUsed;
    uint32_t biClrImportant;
} BITMAPINFOHEADER;
#pragma pack()
#endif

#pragma pack(2)
typedef struct
{
    BITMAPFILEHEADER bfh;
    BITMAPINFOHEADER bih;
} BMPINFO;
#pragma pack()


class bitmap
{
private:
    BMPINFO bmpInfo;
    uint8_t* pixels;

public:
    bitmap(const char* path);
    ~bitmap();

    void save(const char* path, uint16_t bit_count = 24);

    rgb32* getPixel(uint32_t x, uint32_t y) const;
    void setPixel(rgb32* pixel, uint32_t x, uint32_t y);

    uint32_t getWidth() const;
    uint32_t getHeight() const;
    uint16_t bitCount() const;
};

bitmap::bitmap(const char* path) : bmpInfo(), pixels(nullptr)
{
    std::ifstream file(path, std::ios::in | std::ios::binary);

    if (file)
    {
        file.read(reinterpret_cast<char*>(&bmpInfo.bfh), sizeof(bmpInfo.bfh));

        if (bmpInfo.bfh.bfType != 0x4d42)
        {
            throw std::runtime_error("Invalid format. Only bitmaps are supported.");
        }

        file.read(reinterpret_cast<char*>(&bmpInfo.bih), sizeof(bmpInfo.bih));

        if (bmpInfo.bih.biCompression != 0)
        {
            std::cerr << bmpInfo.bih.biCompression << "\n";
            throw std::runtime_error("Invalid bitmap. Only uncompressed bitmaps are supported.");
        }

        if (bmpInfo.bih.biBitCount != 24 && bmpInfo.bih.biBitCount != 32)
        {
            throw std::runtime_error("Invalid bitmap. Only 24bit and 32bit bitmaps are supported.");
        }

        file.seekg(bmpInfo.bfh.bfOffBits, std::ios::beg);

        pixels = new uint8_t[bmpInfo.bfh.bfSize - bmpInfo.bfh.bfOffBits];
        file.read(reinterpret_cast<char*>(&pixels[0]), bmpInfo.bfh.bfSize - bmpInfo.bfh.bfOffBits);


        uint8_t* temp = new uint8_t[bmpInfo.bih.biWidth * bmpInfo.bih.biHeight * sizeof(rgb32)];

        uint8_t* in = pixels;
        rgb32* out = reinterpret_cast<rgb32*>(temp);
        int padding = bmpInfo.bih.biBitCount == 24 ? ((bmpInfo.bih.biSizeImage - bmpInfo.bih.biWidth * bmpInfo.bih.biHeight * 3) / bmpInfo.bih.biHeight) : 0;

        for (int i = 0; i < bmpInfo.bih.biHeight; ++i, in += padding)
        {
            for (int j = 0; j < bmpInfo.bih.biWidth; ++j)
            {

                out->b = *(in++);
                out->g = *(in++);
                out->r = *(in++);
                out->a = bmpInfo.bih.biBitCount == 32 ? *(in++) : 0xFF;
                ++out;
            }
        }

        delete[] pixels;
        pixels = temp;
    }
}

bitmap::~bitmap()
{
    delete[] pixels;
}

void bitmap::save(const char* path, uint16_t bit_count)
{
    std::ofstream file(path, std::ios::out | std::ios::binary);

    if (file)
    {
        bmpInfo.bih.biBitCount = bit_count;
        uint32_t size = ((bmpInfo.bih.biWidth * bmpInfo.bih.biBitCount + 31) / 32) * 4 * bmpInfo.bih.biHeight;
        bmpInfo.bfh.bfSize = bmpInfo.bfh.bfOffBits + size;

        file.write(reinterpret_cast<char*>(&bmpInfo.bfh), sizeof(bmpInfo.bfh));
        file.write(reinterpret_cast<char*>(&bmpInfo.bih), sizeof(bmpInfo.bih));
        file.seekp(bmpInfo.bfh.bfOffBits, std::ios::beg);

        uint8_t* out = NULL;
        rgb32* in = reinterpret_cast<rgb32*>(pixels);
        uint8_t* temp = out = new uint8_t[bmpInfo.bih.biWidth * bmpInfo.bih.biHeight * sizeof(rgb32)];
        int padding = bmpInfo.bih.biBitCount == 24 ? ((bmpInfo.bih.biSizeImage - bmpInfo.bih.biWidth * bmpInfo.bih.biHeight * 3) / bmpInfo.bih.biHeight) : 0;

        for (int i = 0; i < bmpInfo.bih.biHeight; ++i, out += padding)
        {
            for (int j = 0; j < bmpInfo.bih.biWidth; ++j)
            {
                *(out++) = in->b;
                *(out++) = in->g;
                *(out++) = in->r;

                if (bmpInfo.bih.biBitCount == 32)
                {
                    *(out++) = in->a;
                }
                ++in;
            }
        }

        file.write(reinterpret_cast<char*>(&temp[0]), size); //bmpInfo.bfh.bfSize - bmpInfo.bfh.bfOffBits
        delete[] temp;
    }
}

rgb32* bitmap::getPixel(uint32_t x, uint32_t y) const
{
    rgb32* temp = reinterpret_cast<rgb32*>(pixels);
    return &temp[(bmpInfo.bih.biHeight - 1 - y) * bmpInfo.bih.biWidth + x];
}

void bitmap::setPixel(rgb32* pixel, uint32_t x, uint32_t y)
{
    rgb32* temp = reinterpret_cast<rgb32*>(pixels);
    memcpy(&temp[(bmpInfo.bih.biHeight - 1 - y) * bmpInfo.bih.biWidth + x], pixel, sizeof(rgb32));
};

uint32_t bitmap::getWidth() const
{
    return bmpInfo.bih.biWidth;
}

uint32_t bitmap::getHeight() const
{
    return bmpInfo.bih.biHeight;
}

uint16_t bitmap::bitCount() const
{
    return bmpInfo.bih.biBitCount;
}

struct Params
{
    bitmap* bmp;
    uint32_t startW;
    uint32_t endW;
    uint32_t startH;
    uint32_t endH;
};


void blur(bitmap* bmp, int radius)
{
    float rs = ceil(radius * 2.57);
    for (int i = 0; i < bmp->getHeight(); ++i)
    {
        for (int j = 0; j < bmp->getWidth(); ++j)
        {
            double r = 0, g = 0, b = 0;
            double count = 0;

            for (int iy = i - rs; iy < i + rs + 1; ++iy)
            {
                for (int ix = j - rs; ix < j + rs + 1; ++ix)
                {
                    auto x = std::min(static_cast<int>(bmp->getWidth()) - 1, std::max(0, ix));
                    auto y = std::min(static_cast<int>(bmp->getHeight()) - 1, std::max(0, iy));

                    auto dsq = ((ix - j) * (ix - j)) + ((iy - i) * (iy - i));
                    auto wght = std::exp(-dsq / (2.0 * radius * radius)) / (M_PI * 2.0 * radius * radius);

                    rgb32* pixel = bmp->getPixel(x, y);

                    r += pixel->r * wght;
                    g += pixel->g * wght;
                    b += pixel->b * wght;
                    count += wght;
                }
            }

            rgb32* pixel = bmp->getPixel(j, i);
            pixel->r = std::round(r / count);
            pixel->g = std::round(g / count);
            pixel->b = std::round(b / count);
        }
    }
}

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
        }
    }
}

DWORD WINAPI ThreadProc(CONST LPVOID lpParam)
{
    auto params = static_cast<Params *>(lpParam);
    blur_with_threads(5, params);
    ExitThread(0);
}

void ProcessThreads(bitmap* bmp, int threadsCount, int coreCount)
{
    unsigned int lineW = bmp->getWidth() / threadsCount;
    unsigned int remainingPixels = bmp->getWidth() % threadsCount;
    Params* prm = new Params[threadsCount];
    for (size_t i = 0; i < threadsCount; i++)
    {
        prm[i].bmp = bmp;
        prm[i].startH = 0;
        prm[i].endH = bmp->getHeight();
        prm[i].startW = lineW * i;
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
        SetThreadAffinityMask(handles[i], (1 << coreCount) - 1);
    }

    for (int i = 0; i < threadsCount; i++) {
        ResumeThread(handles[i]);
    }

    WaitForMultipleObjects(threadsCount, handles, true, INFINITE);

    delete[] prm;
    delete[] handles;
}

int main(int argc, const char* argv[])
{
    auto start = high_resolution_clock::now();
    bitmap bmp{ "source.bmp" };
    ProcessThreads(&bmp, 10, 1);
    bmp.save("out.bmp");
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stop - start);

    std::cout << duration.count() << std::endl;
    return 0;
}