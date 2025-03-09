#include "render_target.h"

Engine::RenderTarget::RenderTarget(glm::vec2 Size, const std::vector<Attachment>& Attachments)
    : TargetSize(Size), Attachments(Attachments)
{
    glGenFramebuffers(1, &FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);

    std::vector<GLuint> DrawBuffers;
    for (const auto& Attachment : Attachments)
    {
        GLuint Texture;
        glGenTextures(1, &Texture);
        glBindTexture(GL_TEXTURE_2D, Texture);
        glTexImage2D(GL_TEXTURE_2D, 0, Attachment.InternalFormat, TargetSize.x, TargetSize.y, 0, Attachment.Format, Attachment.Type, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + DrawBuffers.size(), GL_TEXTURE_2D, Texture, 0);
        Textures.push_back(Texture);
        DrawBuffers.push_back(GL_COLOR_ATTACHMENT0 + DrawBuffers.size());
    }

    glGenRenderbuffers(1, &RBO);
    glBindRenderbuffer(GL_RENDERBUFFER, RBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, TargetSize.x, TargetSize.y);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);

    glDrawBuffers(DrawBuffers.size(), DrawBuffers.data());

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cerr << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Engine::RenderTarget::~RenderTarget()
{
    glDeleteFramebuffers(1, &FBO);
    for (GLuint Texture : Textures)
        glDeleteTextures(1, &Texture);
    glDeleteRenderbuffers(1, &RBO);
}

void Engine::RenderTarget::Bind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    glViewport(0, 0, TargetSize.x, TargetSize.y);
}

void Engine::RenderTarget::Unbind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Engine::RenderTarget::Resize(glm::vec2 Size)
{
    TargetSize = Size;

    for (size_t i = 0; i < Attachments.size(); ++i)
    {
        glBindTexture(GL_TEXTURE_2D, Textures[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, Attachments[i].InternalFormat, TargetSize.x, TargetSize.y, 0, Attachments[i].Format, Attachments[i].Type, nullptr);
    }

    glBindRenderbuffer(GL_RENDERBUFFER, RBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, TargetSize.x, TargetSize.y);
}
