#pragma once

#ifndef material_h
#define material_h

#include <iostream>
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include "../shaders/shader.h"
#include "../../util/util.h"

namespace Engine
{

    class Material
    {
    public:
        enum class DepthSortingMode
        {
            ReadWrite,
            Write,
            Read,
            None
        };
        enum class BlendingMode
        {
            None,
            AlphaBlend,
            Additive,
            Multiply
        };
        enum class CullingMode
        {
            Back,
            Front,
            None
        };

        Material(const std::string &VertexPath, const std::string &FragmentPath,
                 const std::vector<std::string> &TexturePaths = {},
                 const std::vector<std::pair<GLint, GLint>> &FilterOptions = {});
        ~Material();

        void Bind() const;
        Shader *GetShader();
        void SetShader(const std::string &VertexPath, const std::string &FragmentPath);

        void SetUniform(const std::string &Name, int Value);
        void SetUniform(const std::string &Name, float Value);
        void SetUniform(const std::string &Name, const glm::vec2 &Value);
        void SetUniform(const std::string &Name, const glm::vec3 &Value);
        void SetUniform(const std::string &Name, const glm::vec4 &Value);
        void SetUniform(const std::string &Name, const glm::mat4 &Value);

        void LoadTexture(int Unit, const std::string &TexturePath,
                         GLint MinFilter = GL_LINEAR_MIPMAP_LINEAR,
                         GLint MagFilter = GL_LINEAR);

        void SetTexture(int Unit, unsigned int TextureID);

        void LoadTextures(const std::vector<std::string> &TexturePaths,
                          const std::vector<std::pair<GLint, GLint>> &FilterOptions = {});

        void LoadTexturesFromData(const std::vector<std::tuple<const unsigned char *, int, int, int>> &TextureData,
                                  const std::vector<std::pair<GLint, GLint>> &FilterOptions = {});

        void UnloadTextures();
        void RemoveTexture(int Index);

        void SetDepthSortingMode(DepthSortingMode Mode);
        DepthSortingMode GetDepthSortingMode() const;

        void SetBlendingMode(BlendingMode Mode);
        BlendingMode GetBlendingMode() const;

        void SetCullingMode(CullingMode Mode);
        CullingMode GetCullingMode() const;

        void SetSortOrder(int Order);
        int GetSortOrder() const;

    private:
        Shader Shader;
        std::vector<unsigned int> TextureIDs;
        DepthSortingMode SortingMode = DepthSortingMode::ReadWrite;
        BlendingMode BlendMode = BlendingMode::None;
        CullingMode CullMode = CullingMode::Front;
        int SortOrder = 0;
    };

}
#endif
