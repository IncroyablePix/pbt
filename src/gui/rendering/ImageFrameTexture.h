#pragma once

#include <GL/glew.h>
#include <vector>
#include <string>
#include <expected>
#include <tuple>
#include <filesystem>
#include "Vertex.h"
#include "TextureImage.h"
#include "../../video/Image.h"

class ImageFrameTexture
{
public:
    ImageFrameTexture();
    ~ImageFrameTexture();

    void Draw(int windowWidth, int windowHeight, Image& image);
    
    static std::expected<TextureImage, std::string> LoadTextureFromFile(const std::filesystem::path& path);

private:
    std::pair<int, int> ResizeImage(int windowWidth, int windowHeight, int imageWidth, int imageHeight);
    unsigned int CreateTexture();

    unsigned int m_textureId;
};
