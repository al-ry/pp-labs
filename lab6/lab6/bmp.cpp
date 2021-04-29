#include "bmp.h"

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
