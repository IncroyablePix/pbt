#include "VideoPlayer.h"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <nfd.h>
#include <iostream>

VideoPlayer::VideoPlayer() :
    m_renderer(std::make_unique<OpenGL33Renderer>()),
    m_texture(std::make_unique<ImageFrameTexture>())
{
    NFD_Init();
    AddImage("play", "./rsc/play.png");
    AddImage("pause", "./rsc/pause.png");
    AddImage("fast-forward", "./rsc/fast-forward.png");
    AddImage("fast-backward", "./rsc/fast-backward.png");
}

VideoPlayer::~VideoPlayer()
{
    NFD_Quit();

    for (const auto& [id, image] : m_icons)
    {
        // TODO : Free from GPU
    }
}

void VideoPlayer::Update(double deltaTime)
{
    if (m_feed)
    {

    }
}

void VideoPlayer::RenderUI(int width, int height)
{
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui::Begin("Controls", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);
    if (IsEmpty())
        ImGui::Text("No file");
    else 
        ImGui::Text("%s", m_feed->GetPath().string().c_str());

    ImGui::SameLine();
    if (ImGui::Button("Open"))
    {
        nfdchar_t *outPath;
        nfdfilteritem_t filterItem[2] = { { "Video files", "mp4,mkv,avi,png" } };
        nfdresult_t result = NFD_OpenDialog(&outPath, filterItem, 2, nullptr);
        if (result == NFD_OKAY)
        {
            try 
            {
                m_feed = std::make_unique<VideoFeed>(std::filesystem::path(outPath));
            }
            catch (const std::runtime_error& e)
            {
                std::cout << e.what() << std::endl;
            }

            NFD_FreePath(outPath);
        }
    }

    if (ImGui::ImageButton((void*)(intptr_t)((*GetImage("fast-backward")).Id), ImVec2(32, 32)))
    {
    }
    ImGui::SameLine();

    if (ImGui::ImageButton((void*)(intptr_t)((*GetImage("play")).Id), ImVec2(32, 32)))
    {
        if (m_feed)
            m_feed->Play();
    }
    ImGui::SameLine();

    if (ImGui::ImageButton((void*)(intptr_t)((*GetImage("pause")).Id), ImVec2(32, 32)))
    {
        if (m_feed)
            m_feed->Pause();
    }
    ImGui::SameLine();

    if (ImGui::ImageButton((void*)(intptr_t)((*GetImage("fast-forward")).Id), ImVec2(32, 32)))
    {
    }

    ImGui::End();
}

void VideoPlayer::Render(int width, int height)
{
    m_renderer->Clear();
    if (!IsEmpty())
    {
        auto image = m_feed->GetImage();

        if (m_filter)
            (*m_filter)(image);

        m_texture->Draw(width, height, image);
    }

    glViewport(0, 0, width, height);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    RenderUI(width, height);
    ImGui::Render();
    
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

std::expected<TextureImage, std::string> VideoPlayer::AddImage(std::string id, const std::filesystem::path& path)
{
    if (m_icons.contains(id))
    {
        return m_icons[id];
    }

    auto result = ImageFrameTexture::LoadTextureFromFile(path);
    if (result.has_value())
    {
        m_icons[std::move(id)] = *result;
        return *result;
    }

    return std::unexpected(result.error());
}
    
std::expected<TextureImage, std::string> VideoPlayer::GetImage(const std::string& id)
{
    if (m_icons.contains(id))
    {
        return m_icons[id];
    }

    return std::unexpected("No such image!");
}

void VideoPlayer::SetFilter(std::unique_ptr<IImageFilter> imageFilter)
{
    m_filter = std::move(imageFilter);
}
