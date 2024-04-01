#include "VideoFeed.h"
#include <stdexcept>
#include <iostream>
#include <stb_image.h>

VideoFeed::VideoFeed(std::filesystem::path path) :
        m_path(std::move(path))
{
    m_formatContext = nullptr;

    if (avformat_open_input(&m_formatContext, m_path.string().c_str(), nullptr, nullptr) < 0) {
        abort();
    }

    if (avformat_find_stream_info(m_formatContext, nullptr) < 0) {
        abort();
    }

    av_dump_format(m_formatContext, -1, m_path.string().c_str(), 0);

    AVCodec *codec{nullptr};

    int videoIndex = av_find_best_stream(m_formatContext, AVMEDIA_TYPE_VIDEO, -1, -1,
                                         (const AVCodec **)(&codec), 0);
    if (videoIndex < 0)
    {
        abort();
    }

    m_videoIndex = videoIndex;

    m_codecContext = avcodec_alloc_context3(codec);
    if (!m_codecContext)
    {
        abort();
    }

    avcodec_parameters_to_context(m_codecContext, m_formatContext->streams[videoIndex]->codecpar);

    int ret = avcodec_open2(m_codecContext, codec, nullptr);
    if (ret < 0)
    {
        abort();
    }

    m_image =
    {
            ImageType::RGB,
            m_codecContext->width,
            m_codecContext->height,
            std::vector<std::byte>(m_codecContext->width * m_codecContext->height * static_cast<uint32_t>(ImageType::RGB))
    };

    m_rescaler = sws_getContext(m_codecContext->width, m_codecContext->height, m_codecContext->pix_fmt, m_codecContext->width, m_codecContext->height, AV_PIX_FMT_RGB24, SWS_BICUBIC, nullptr, nullptr, nullptr);
    m_play = false;

    m_thread = std::thread([this]() { StartDecoding(); });
}

VideoFeed::~VideoFeed()
{
    Stop();

    m_thread.join();

    avformat_close_input(&m_formatContext);
    avcodec_free_context(&m_codecContext);
    sws_freeContext(m_rescaler);
}

void VideoFeed::StartDecoding()
{
    AVPacket *packet = av_packet_alloc();
    AVFrame *frame = av_frame_alloc();
    AVFrame *outFrame = av_frame_alloc();

    while ((av_read_frame(m_formatContext, packet)) >= 0)
    {
        if (!m_play)
        {
            std::unique_lock lock(m_playMutex);
            m_playCondition.wait(lock, [this] { return m_play || m_stopped; });
        }

        if (m_stopped)
        {
            return;
        }

        if (packet->stream_index != m_videoIndex)
        {
            av_packet_unref(packet);
            continue;
        }

        bool okay = false;

        while (!okay)
        {
            int ret = avcodec_send_packet(m_codecContext, packet);

            if (ret >= 0)
            {
                okay = true;
                av_packet_unref(packet);
            }

            while(avcodec_receive_frame(m_codecContext, frame) >= 0)
            {
                outFrame->width = frame->width;
                outFrame->height = frame->height;
                outFrame->format = AV_PIX_FMT_RGB24;

                av_frame_get_buffer(outFrame, 0);

                int result = sws_scale(m_rescaler, frame->data, frame->linesize, 0, frame->height, outFrame->data, outFrame->linesize);

                av_frame_unref(frame);

                if (result < 0)
                {
                    throw std::runtime_error("Failed to decode frame\n");
                }

                {
                    std::unique_lock lock(m_dataMutex);
                    memcpy(m_image.Data.data(), outFrame->data[0], outFrame->linesize[0] * outFrame->height);
                }

                av_frame_unref(outFrame);

                std::this_thread::sleep_for(std::chrono::milliseconds(30));
            }
        }
    }

    av_frame_free(&frame);
    av_frame_free(&outFrame);
    av_packet_free(&packet);
}

void VideoFeed::Play()
{
    m_play = true;
    m_playCondition.notify_one();
}

void VideoFeed::Pause()
{
    m_play = false;
    m_playCondition.notify_one();
}

/*uint8_t *VideoFeed::GetFrameData()
{
    std::unique_lock lock(m_dataMutex);
    return m_frameData;
}*/

void VideoFeed::Stop()
{
    m_stopped = true;
    m_playCondition.notify_one();
}
