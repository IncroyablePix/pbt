#pragma once

#include "rendering/OpenGL33Renderer.h"
#include "rendering/ImageFrameTexture.h"
#include "../video/VideoFeed.h"
#include "../video/IImageFilter.h"
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <expected>
#include <stdexcept>

class VideoPlayer
{
    public:
        VideoPlayer();
        ~VideoPlayer();
	    void Render(int width, int height);
        void Update(double deltaTime);
        void RenderUI(int width, int height);
        [[nodiscard]] bool IsEmpty() const { return !m_feed; };
        void SetFilter(std::unique_ptr<IImageFilter> imageFilter);

    private:
        std::expected<TextureImage, std::string> AddImage(std::string id, const std::filesystem::path& path);
        std::expected<TextureImage, std::string> GetImage(const std::string& id);

    private:
        std::unique_ptr<OpenGL33Renderer> m_renderer;
        std::unique_ptr<ImageFrameTexture> m_texture;
        std::unique_ptr<VideoFeed> m_feed;
        std::unique_ptr<IImageFilter> m_filter;

        std::unordered_map<std::string, TextureImage> m_icons;

        long m_timeSum { 0 };
        int64_t m_pts { 0 };
        long m_currentFrame { 0 };
        long m_deltaTime { 0 };
};