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
        Material(
            const std::string& VertexPath,
            const std::string& FragmentPath,
            const std::vector<std::string>& TexturePaths,
            const std::vector<std::pair<GLint, GLint>>& FilterOptions = {}); 

        ~Material();

        void Bind() const;

        void SetUniform(const std::string &Name, int Value);
        void SetUniform(const std::string &Name, float Value);
        void SetUniform(const std::string &Name, const glm::vec3 &Value);
        void SetUniform(const std::string &Name, const glm::vec4 &Value);
        void SetUniform(const std::string &Name, const glm::mat4 &Value);
        void SetShader(const std::string &VertexPath, const std::string &FragmentPath);
        void SetTexture(int Unit, unsigned int TextureID);
        void LoadTexturesFromData(
            const std::vector<std::tuple<const unsigned char*, int, int, int>>& TextureData,
            const std::vector<std::pair<GLint, GLint>>& FilterOptions);
        void LoadTexture(int Unit, const std::string& TexturePath, GLint MinFilter = GL_LINEAR_MIPMAP_LINEAR, GLint MagFilter = GL_LINEAR);
        void LoadTextures(const std::vector<std::string>& TexturePaths, const std::vector<std::pair<GLint, GLint>>& FilterOptions);
        void UnloadTextures();
        Shader* GetShader();
        void RemoveTexture(int Index);

    private:
        Shader Shader;
        std::vector<unsigned int> TextureIDs;
    };
};

#endif
