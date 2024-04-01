#pragma once

#include <vector>
#include <cstddef>

enum class ImageType : uint32_t
{
    RGB = 3,
    RGBA = 4,
};

struct Image
{
    ImageType Mode;
    int Width;
    int Height;
    std::vector<std::byte> Data;
};