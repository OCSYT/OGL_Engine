#ifndef RENDER_TARGET_H
#define RENDER_TARGET_H

#include <glad/glad.h>
#include <iostream>
#include <glm/glm.hpp>

class RenderTarget
{
public:
    GLuint FBO; // Framebuffer Object
    GLuint AlbedoTexture;   // Texture for albedo (color)
    GLuint NormalTexture;   // Texture for normals
    GLuint PositionTexture; // Texture for world space positions
    GLuint SpecularTexture;   // Texture for normals
    GLuint MetallicTexture; // Texture for world space positions
    GLuint RBO;             // Renderbuffer Object for depth and stencil
    glm::vec2 TargetSize;   // Size of the target (window or framebuffer size)

    // Constructor that initializes the framebuffer and textures
    RenderTarget(glm::vec2 Size);

    // Destructor to clean up resources
    ~RenderTarget();

    // Binds the framebuffer to render to it
    void Bind();

    // Unbinds the framebuffer to render to the default framebuffer
    void Unbind();

    // Resizes the textures and renderbuffer
    void Resize(glm::vec2 Size);
};

#endif
