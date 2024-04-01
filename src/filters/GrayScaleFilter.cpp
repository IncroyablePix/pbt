#include <algorithm>
#include <stdexcept>
#include "GrayScaleFilter.h"

void GrayScaleFilter::operator()(Image &image)
{
    for (int i = 0; i < image.Data.size(); i += static_cast<uint32_t>(image.Mode))
    {
        auto r = static_cast<uint8_t>(image.Data[i + 0]);
        auto g = static_cast<uint8_t>(image.Data[i + 1]);
        auto b = static_cast<uint8_t>(image.Data[i + 2]);
        auto gray = static_cast<uint8_t>((r + g + b) / 3);
        image.Data[i + 0] = std::byte(gray);
        image.Data[i + 1] = std::byte(gray);
        image.Data[i + 2] = std::byte(gray);
    }
}
