#pragma once

#ifndef material_h
#define material_h

#include <string>
#include <vector>
#include <glm/glm.hpp>
#include "../../rendering/shaders/shader.h"
#include "../../util/util.h"

class Material {
public:
    Material(const std::string& VertexPath, const std::string& FragmentPath, const std::vector<std::string>& TexturePaths);

    ~Material();

    void Bind() const;

    void SetUniform(const std::string &Name, int Value);
    void SetUniform(const std::string &Name, float Value);
    void SetUniform(const std::string &Name, const glm::vec3 &Value);
    void SetUniform(const std::string &Name, const glm::vec4 &Value);
    void SetUniform(const std::string &Name, const glm::mat4 &Value);

    void SetTexture(int Unit, const std::string& TexturePath);

    void SetShader(const std::string& VertexPath, const std::string& FragmentPath);

    void LoadTextures(const std::vector<std::string>& TexturePaths);

    void UnloadTextures();
    Shader GetShader();
    void RemoveTexture(int Index);
    void ReloadTexture(int Index, const std::string& NewTexturePath);

private:
    Shader Shader;
    std::vector<unsigned int> TextureIDs;
};

#endif