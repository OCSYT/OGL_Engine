#ifndef render_target_h
#define render_target_h

#include <glad/glad.h>
#include <iostream>
#include <glm/glm.hpp>
#include <vector>

namespace Engine
{
    class RenderTarget
    {
    public:
        struct Attachment
        {
            GLenum InternalFormat;
            GLenum Format;
            GLenum Type;

            Attachment(GLenum internalFormat, GLenum format, GLenum type)
                : InternalFormat(internalFormat), Format(format), Type(type) {}
        };
        GLuint FBO;
        GLuint RBO;
        glm::vec2 TargetSize;
        std::vector<GLuint> Textures;
        std::vector<Attachment> Attachments;

        RenderTarget(glm::vec2 Size, const std::vector<Attachment> &Attachments);
        ~RenderTarget();
        void Bind();
        void Unbind();
        void Resize(glm::vec2 Size);
    };
};
#endif
