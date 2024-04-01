#pragma once

#include "../video/IImageFilter.h"
#include <torch/script.h>
#include <filesystem>

class SuperPointFilter : public IImageFilter
{
public:
    SuperPointFilter(const std::filesystem::path& modelPath);
    ~SuperPointFilter() override = default;

    void operator()(Image &image) override;

private:

};
