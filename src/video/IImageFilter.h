#pragma once

#include "Image.h"

class IImageFilter
{
public:
    virtual ~IImageFilter() = default;
    virtual void operator()(Image& image) = 0;
};
