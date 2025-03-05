#pragma once

#ifndef util_h
#define util_h

#include <iostream>
#include <string>
#include <filesystem>
#include <vector>
#include <windows.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <stb_image.h>

namespace Engine
{
    class Util
    {
    public:
        struct MeshData
        {
            unsigned int VAO, VBO, EBO;
            unsigned int IndexCount;
        };

        struct Mesh
        {
            std::vector<MeshData> Meshes;
        };

        static std::string GetExecutablePath();
        static unsigned int LoadTexture(std::string Path);
        static unsigned int LoadTextureFromData(const unsigned char* Data, int Width, int Height, int NumChannels);
        static void UnloadTexture(unsigned int &TextureID);
    };
};

#endif
