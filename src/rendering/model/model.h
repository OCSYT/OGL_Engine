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
#include <algorithm>
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
            int MaterialIndex;
        };


        struct LightData {
            enum class LightType { AMBIENT, DIRECTIONAL, POINT, SPOT };
            
            LightType Type;
            glm::vec3 Position;
            glm::vec3 Direction;
            glm::vec3 Color;
            float Intensity;
            
            // Spot and Point light parameters
            float Constant;
            float Linear;
            float Quadratic;
            float InnerCone;
            float OuterCone;
        };
        

        struct MaterialData {
            glm::vec4 DiffuseColor;      // Diffuse color
            glm::vec4 AmbientColor;      // Ambient color
            glm::vec4 SpecularColor;     // Specular color
            glm::vec4 EmissiveColor;     // Emissive color
            float Shininess;             // Shininess (specular exponent)
            float Transparency;          // Transparency factor
        
            std::vector<std::string> DiffuseTextures;   // Diffuse texture paths
            std::vector<std::string> SpecularTextures;  // Specular texture paths
            std::vector<std::string> NormalTextures;    // Normal map texture paths
            std::vector<std::string> HeightTextures;    // Height map texture paths
            std::vector<std::string> AmbientTextures;   // Ambient texture paths
        
            // Constructor to initialize default values
            MaterialData()
                : DiffuseColor(glm::vec4(1.0f)), // Default white color
                  AmbientColor(glm::vec4(0.2f)),  // Default dim ambient color
                  SpecularColor(glm::vec4(0.0f)), // Default black specular color
                  EmissiveColor(glm::vec4(0.0f)), // Default black emissive color
                  Shininess(32.0f),               // Default shininess value
                  Transparency(1.0f) {}           // Default transparency (fully opaque)
        };
        struct Mesh
        {
            std::vector<MeshData> Meshes;
            std::vector<MaterialData> MaterialData;
            std::vector<LightData> Lights;
        
            ~Mesh();
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
