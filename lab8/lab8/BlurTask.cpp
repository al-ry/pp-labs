#include "ITask.h"
#include "BlurTask.h"

BlurTask::BlurTask(int radius,const BlurParams& blurPrms)
{
    m_radius = radius;
    m_params = blurPrms;
}

void BlurTask::Execute()
{
    float rs = ceil(m_radius * 2.57);
    for (int i = m_params.startHeight; i < m_params.endHeight; ++i)
    {
        for (int j = m_params.startWidth; j < m_params.endWidth; ++j)
        {
            double r = 0, g = 0, b = 0;
            double count = 0;

            for (int iy = i - rs; iy < i + rs + 1; ++iy)
            {
                for (int ix = j - rs; ix < j + rs + 1; ++ix)
                {
                    auto x = std::min(static_cast<int>(m_params.endWidth) - 1, std::max(0, ix));
                    auto y = std::min(static_cast<int>(m_params.endHeight) - 1, std::max(0, iy));

                    auto dsq = ((ix - j) * (ix - j)) + ((iy - i) * (iy - i));
                    auto wght = std::exp(-dsq / (2.0 * m_radius * m_radius)) / (M_PI * 2.0 * m_radius * m_radius);

                    rgb32* pixel = m_params.bmp->getPixel(x, y);

                    r += pixel->r * wght;
                    g += pixel->g * wght;
                    b += pixel->b * wght;
                    count += wght;
                }
            }

            rgb32* pixel = m_params.bmp->getPixel(j, i);
            pixel->r = std::round(r / count);
            pixel->g = std::round(g / count);
            pixel->b = std::round(b / count);

        }
    }
}


