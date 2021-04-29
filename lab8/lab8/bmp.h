#pragma once
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