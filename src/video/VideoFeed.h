#pragma once

#include <memory>
#include <filesystem>
#include <optional>
#include <vector>
#include <cstddef>
#include <thread>
#include <mutex>
#include "Image.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <inttypes.h>
}

class VideoFeed
{
public:
    VideoFeed(std::filesystem::path path);

    ~VideoFeed();

    [[nodiscard]] const std::filesystem::path &GetPath() const
    { return m_path; }

    [[nodiscard]] Image& GetImage()
    {
        std::unique_lock lock(m_dataMutex);
        return m_image;
    }

    /*[[nodiscard]] uint8_t *GetFrameData();

    [[nodiscard]] int GetWidth() const
    { return m_width; }

    [[nodiscard]] int GetHeight() const
    { return m_height; } */

    void Play();
    void Stop();
    void Pause();

private:
    void StartDecoding();

private:
    std::filesystem::path m_path;

    Image m_image;
    /*int m_width;
    int m_height;

    uint8_t *m_frameData;*/

    std::atomic<bool> m_stopped { false };

    AVFormatContext *m_formatContext;
    AVCodecContext *m_codecContext;
    int m_videoIndex;
    SwsContext *m_rescaler;

    std::mutex m_dataMutex;
    std::mutex m_playMutex;
    std::thread m_thread;
    std::atomic<bool> m_play{false};
    std::condition_variable m_playCondition;
};
