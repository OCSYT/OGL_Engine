#pragma once

#ifndef model_h
#define model_h

#include <iostream>
#include <string>
#include <vector>
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
            std::vector<aiMaterial*> MaterialData;
        };

        struct ModelInstance
        {
            Mesh ModelMesh;
            std::vector<Material*> Materials;
            glm::mat4 Transform;
        
            ModelInstance(const Mesh& modelMesh = Mesh(),
                          const std::vector<Material*>& materials = {}, 
                          const glm::mat4& transform = glm::mat4(1.0f))
                : ModelMesh(modelMesh), Materials(materials), Transform(transform)
            {
            }
        };
        

        static void UnloadModelInstance(ModelInstance& instance);
        static Mesh LoadMesh(std::string Path);
        static void UnloadMesh(Mesh &Mesh);
        static void DrawModel(const MeshData &Mesh, class Material *MaterialPtr, const glm::mat4 &ModelMatrix, Camera *MainCamera);
        static void DrawMesh(const Mesh &ModelMesh, const std::vector<Material *> &Materials, const glm::mat4 &ModelMatrix, Camera *MainCamera);
        static void DrawModelInstances(const std::vector<ModelInstance> &ModelInstances, Camera *MainCamera);
    };
};

#endif
