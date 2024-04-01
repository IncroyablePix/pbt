#pragma once

#include <GL/glew.h>

class OpenGL33Renderer 
{
public:
    OpenGL33Renderer();
    void Clear();
    
private:
    static bool s_glewInitialized;
    static void GlewInitAttempt();
};
