#include "OpenGL33Renderer.h"
#include <stdexcept>

bool OpenGL33Renderer::s_glewInitialized = false;

OpenGL33Renderer::OpenGL33Renderer()
{
    glClearColor(0.0, 0.0, 0.0, 0.0);
    /*glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glCullFace(GL_FRONT);*/

    GlewInitAttempt();
}

void OpenGL33Renderer::Clear()
{
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void OpenGL33Renderer::GlewInitAttempt()
{
    if(!s_glewInitialized)
    {
        glewExperimental = GL_TRUE;
        if (glewInit() != GLEW_OK)
            throw std::runtime_error("Failed to initialize GLEW.");

        /*printf("----------------------------------------------------------------\n");
        printf("Graphics Successfully Initialized\n");
        printf("OpenGL Info\n");
        printf("    Version: %s\n", glGetString(GL_VERSION));
        printf("     Vendor: %s\n", glGetString(GL_VENDOR));
        printf("   Renderer: %s\n", glGetString(GL_RENDERER));
        printf("    Shading: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
        printf("----------------------------------------------------------------\n");*/

        s_glewInitialized = true;
    }
}