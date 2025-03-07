#ifndef RENDER_TARGET_H
#define RENDER_TARGET_H

#include <glad/glad.h>
#include <iostream>
#include <glm/glm.hpp>

class RenderTarget
{
public:
    GLuint FBO;
    GLuint AlbedoTexture;
    GLuint NormalTexture;
    GLuint PositionTexture;
    GLuint RoughnessTexture;
    GLuint MetallicTexture;
    GLuint EmissionTexture;
    GLuint DepthTexture;
    GLuint RBO;
    glm::vec2 TargetSize;

    RenderTarget(glm::vec2 Size);
    ~RenderTarget();
    void Bind();
    void Unbind();
    void Resize(glm::vec2 Size);
};

#endif
