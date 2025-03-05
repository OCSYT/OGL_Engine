#pragma once

#ifndef model_h
#define model_h

#include <iostream>
#include <string>
#include <filesystem>
#include <vector>
#include <windows.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/glm.hpp>
#include "../../util/util.h"
#include "../materials/material.h"
#include "../camera/camera.h"

namespace Engine
{
    class Model
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

        static Mesh LoadMesh(std::string Path);
        static void UnloadMesh(Mesh &Mesh);
        static void DrawModel(const MeshData &Mesh, class Material *MaterialPtr, const glm::mat4 &ModelMatrix, Camera *MainCamera);
        static void DrawMesh(const Mesh& ModelMesh, const std::vector<Material*>& Materials, const glm::mat4& ModelMatrix, Camera* MainCamera);
    };
};

#endif
