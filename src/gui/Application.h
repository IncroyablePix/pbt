#pragma once

#include "rendering/ImageFrameTexture.h"
#include "rendering/OpenGL33Renderer.h"
#include "VideoPlayer.h"
#include <memory>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>

class Application
{
public:
    Application();
    void Run();

private:
    GLFWwindow* m_window { nullptr };
    double m_previousTime { 0.0 };
    std::unique_ptr<VideoPlayer> m_videoPlayer;
};