#pragma once
  
#include <string>
#include <vector>
#include "../../rendering/shaders/Shader.h"
#include "../../util/Util.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <iostream>
#include <filesystem>

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

    void LoadTextures(const std::vector<std::string>& TexturePaths);
    void UnloadTextures();

private:
    Shader Shader;
    std::vector<unsigned int> TextureIDs;
};
