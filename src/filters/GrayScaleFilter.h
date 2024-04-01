#ifndef SPP_GRAYSCALEFILTER_H
#define SPP_GRAYSCALEFILTER_H


#include "../video/IImageFilter.h"

class GrayScaleFilter : public IImageFilter
{
public:
    ~GrayScaleFilter() override = default;
    void operator()(Image &image) override;
};


#endif //SPP_GRAYSCALEFILTER_H
