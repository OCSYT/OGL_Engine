#pragma once

#ifndef shader_h
#define shader_h

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <unordered_map>
#include <glad/glad.h>
#include <filesystem>
#include <windows.h>
#include "../../util/util.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>


namespace Engine
{
    class Shader
    {
    public:
        unsigned int ID;

        Shader(const std::string &vertexPath, const std::string &fragmentPath);
        ~Shader();

        void Bind() const;
        void Unbind() const;
        void Unload();

        int GetUniformLocation(const std::string &name);

        // Set Uniforms
        void SetUniform(const std::string &name, int value);
        void SetUniform(const std::string &name, float value);
        void SetUniform(const std::string &name, const glm::vec2 &value);
        void SetUniform(const std::string &name, const glm::vec3 &value);
        void SetUniform(const std::string &name, const glm::vec4 &value);
        void SetUniform(const std::string &name, const glm::mat4 &value);
        void Engine::Shader::ListUniforms();

    private:
        std::unordered_map<std::string, int> UniformCache;

        unsigned int CreateShader(unsigned int shaderType, const std::string &shaderSource);
        bool CompileShader(unsigned int shaderId);
        unsigned int CreateProgram(unsigned int vertexShaderId, unsigned int fragmentShaderId);
        bool LinkProgram(unsigned int programId);

        std::string LoadShaderSource(const std::string &filePath);
    };
};
#endif