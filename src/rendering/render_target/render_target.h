#ifndef render_target_h
#define render_target_h

#include <glad/glad.h>
#include <iostream>
#include <glm/glm.hpp>

class RenderTarget
{
public:
    GLuint FBO;
    GLuint Texture;
    GLuint RBO;
    glm::vec2 TargetSize;

    RenderTarget(glm::vec2 Size);
    ~RenderTarget();

    void Bind();
    void Unbind();
    void Resize(glm::vec2 Size);
};

#endif
