#include "render_target.h"

RenderTarget::RenderTarget(glm::vec2 Size) : TargetSize(Size)
{
    glGenFramebuffers(1, &FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);

    glGenTextures(1, &AlbedoTexture);
    glBindTexture(GL_TEXTURE_2D, AlbedoTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, TargetSize.x, TargetSize.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, AlbedoTexture, 0);

    glGenTextures(1, &NormalTexture);
    glBindTexture(GL_TEXTURE_2D, NormalTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, TargetSize.x, TargetSize.y, 0, GL_RGB, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, NormalTexture, 0);

    glGenTextures(1, &PositionTexture);
    glBindTexture(GL_TEXTURE_2D, PositionTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, TargetSize.x, TargetSize.y, 0, GL_RGB, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, PositionTexture, 0);

    glGenTextures(1, &DepthTexture);
    glBindTexture(GL_TEXTURE_2D, DepthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R16F, TargetSize.x, TargetSize.y, 0, GL_RED, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, DepthTexture, 0);

    glGenTextures(1, &MetallicTexture);
    glBindTexture(GL_TEXTURE_2D, MetallicTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R16F, TargetSize.x, TargetSize.y, 0, GL_RED, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, MetallicTexture, 0);

    glGenTextures(1, &RoughnessTexture);
    glBindTexture(GL_TEXTURE_2D, RoughnessTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R16F, TargetSize.x, TargetSize.y, 0, GL_RED, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT5, GL_TEXTURE_2D, RoughnessTexture, 0);

    glGenTextures(1, &EmissionTexture);
    glBindTexture(GL_TEXTURE_2D, EmissionTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, TargetSize.x, TargetSize.y, 0, GL_RGB, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT6, GL_TEXTURE_2D, EmissionTexture, 0);


    glGenRenderbuffers(1, &RBO);
    glBindRenderbuffer(GL_RENDERBUFFER, RBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, TargetSize.x, TargetSize.y);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);

    GLuint attachments[7] = {
        GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT5, GL_COLOR_ATTACHMENT6
    };
    glDrawBuffers(7, attachments);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cerr << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

RenderTarget::~RenderTarget()
{
    glDeleteFramebuffers(1, &FBO);
    glDeleteTextures(1, &AlbedoTexture);
    glDeleteTextures(1, &NormalTexture);
    glDeleteTextures(1, &PositionTexture);
    glDeleteTextures(1, &MetallicTexture);
    glDeleteTextures(1, &RoughnessTexture);
    glDeleteTextures(1, &EmissionTexture);
    glDeleteTextures(1, &DepthTexture);
    glDeleteRenderbuffers(1, &RBO);
}

void RenderTarget::Bind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    glViewport(0, 0, TargetSize.x, TargetSize.y);
}

void RenderTarget::Unbind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderTarget::Resize(glm::vec2 Size)
{
    TargetSize = Size;

    glBindTexture(GL_TEXTURE_2D, AlbedoTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, TargetSize.x, TargetSize.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    glBindTexture(GL_TEXTURE_2D, NormalTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, TargetSize.x, TargetSize.y, 0, GL_RGB, GL_FLOAT, nullptr);

    glBindTexture(GL_TEXTURE_2D, PositionTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, TargetSize.x, TargetSize.y, 0, GL_RGB, GL_FLOAT, nullptr);

    glBindTexture(GL_TEXTURE_2D, MetallicTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R16F, TargetSize.x, TargetSize.y, 0, GL_RED, GL_FLOAT, nullptr);

    glBindTexture(GL_TEXTURE_2D, RoughnessTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R16F, TargetSize.x, TargetSize.y, 0, GL_RED, GL_FLOAT, nullptr);

    glBindTexture(GL_TEXTURE_2D, EmissionTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, TargetSize.x, TargetSize.y, 0, GL_RGB, GL_FLOAT, nullptr);

    glBindTexture(GL_TEXTURE_2D, DepthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R16F, TargetSize.x, TargetSize.y, 0, GL_RED, GL_FLOAT, nullptr);

    glBindRenderbuffer(GL_RENDERBUFFER, RBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, TargetSize.x, TargetSize.y);
}
