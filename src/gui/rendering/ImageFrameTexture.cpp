#include "ImageFrameTexture.h"
#include "../../video/Image.h"
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

ImageFrameTexture::ImageFrameTexture() :
    m_textureId(CreateTexture())
{
    
}

ImageFrameTexture::~ImageFrameTexture()
{
    glDeleteTextures(1, &m_textureId);
}

void ImageFrameTexture::Draw(int windowWidth, int windowHeight, Image& image)
{
    const auto imageWidth = image.Width;
    const auto imageHeight = image.Height;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(10, windowWidth, windowHeight, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);

    // Texture bind
    glBindTexture(GL_TEXTURE_2D, m_textureId);
    glTexImage2D(GL_TEXTURE_2D, 0, (image.Mode == ImageType::RGB) ? GL_RGB : GL_RGBA, imageWidth, imageHeight, 0, (image.Mode == ImageType::RGB) ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, image.Data.data());

    glEnable(GL_TEXTURE_2D);

    glBegin(GL_QUADS);

    const auto [w, h] = ResizeImage(windowWidth, windowHeight, imageWidth, imageHeight);

    if (w == windowWidth && h < windowHeight)
    {
        auto yOffset = (windowHeight - h) / 2;
        glTexCoord2d(0,0); glVertex2i(0, yOffset);
        glTexCoord2d(1,0); glVertex2i(w, yOffset);
        glTexCoord2d(1,1); glVertex2i(w, h + yOffset);
        glTexCoord2d(0,1); glVertex2i(0, h + yOffset);
    }
    else if (h == windowHeight && w < windowWidth)
    {
        auto xOffset = (windowWidth - w) / 2;
        glTexCoord2d(0,0); glVertex2i(xOffset, 0);
        glTexCoord2d(1,0); glVertex2i(w + xOffset, 0);
        glTexCoord2d(1,1); glVertex2i(w + xOffset, h);
        glTexCoord2d(0,1); glVertex2i(xOffset, h);
    }
    else
    {
        glTexCoord2d(0,0); glVertex2i(0, 0);
        glTexCoord2d(1,0); glVertex2i(w, 0);
        glTexCoord2d(1,1); glVertex2i(w, h);
        glTexCoord2d(0,1); glVertex2i(0, h);
    }

    glEnd();
    glDisable(GL_TEXTURE_2D);

    glActiveTexture(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    /*Bind();

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    Unbind();*/
}

unsigned int ImageFrameTexture::CreateTexture()
{
    unsigned int textureId;
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    return textureId;
}

std::expected<TextureImage, std::string> ImageFrameTexture::LoadTextureFromFile(const std::filesystem::path& path)
{
// Load from file
    int imageWidth = 0;
    int imageHeight = 0;
    unsigned char* imageData = stbi_load(path.string().c_str(), &imageWidth, &imageHeight, nullptr, 4);
    
    if (!imageData)
        return std::unexpected("Could not load file: " + path.string() + "!");

    // Create a OpenGL texture identifier
    GLuint imageTexture;
    glGenTextures(1, &imageTexture);
    glBindTexture(GL_TEXTURE_2D, imageTexture);

    // Setup filtering parameters for display
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // This is required on WebGL for non power-of-two textures
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Same

    // Upload pixels into texture
#if defined(GL_UNPACK_ROW_LENGTH) && !defined(__EMSCRIPTEN__)
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#endif
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);
    stbi_image_free(imageData);

    return TextureImage {imageTexture, imageWidth, imageHeight };
}

std::pair<int, int> ImageFrameTexture::ResizeImage(int windowWidth, int windowHeight, int imageWidth, int imageHeight)
{
    // Calculate aspect ratios
    float windowRatio = static_cast<float>(windowWidth) / static_cast<float>(windowHeight);
    float imageRatio = static_cast<float>(imageWidth) / static_cast<float>(imageHeight);

    // Calculate scaling factors
    float widthScale = static_cast<float>(windowWidth) / static_cast<float>(imageWidth);
    float heightScale = static_cast<float>(windowHeight) / static_cast<float>(imageHeight);

    int newWidth, newHeight;

    if (widthScale < heightScale)
    {
        // Resize based on width scale
        newWidth = windowWidth;
        newHeight = static_cast<int>(imageHeight * widthScale);
    }
    else
    {
        // Resize based on height scale
        newWidth = static_cast<int>(imageWidth * heightScale);
        newHeight = windowHeight;
    }

    return std::make_pair(newWidth, newHeight);
}
