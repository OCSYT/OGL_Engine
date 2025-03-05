#pragma once

#ifndef sprite_h
#define sprite_h

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "../materials/material.h"

namespace Engine
{
    class Sprite
    {
    public:
        Sprite(Material* material, const glm::vec2 &position,
               const glm::vec2 &size, unsigned int *screenWidth, unsigned int *screenHeight);
        ~Sprite();

        void Render();

        void SetPosition(const glm::vec2 &position);
        void SetSize(const glm::vec2 &size);
        void SetMaterial(Material* material);
        void SetUV(const glm::vec4 &uv); // NEW: Set custom UV coordinates

        Material* GetMaterial() const;
        glm::vec2 GetPosition() const;
        glm::vec2 GetSize() const;

    private:
        Material* MaterialInstance;
        glm::vec2 Position;
        glm::vec2 Size;

        unsigned int *ScreenWidth;
        unsigned int *ScreenHeight;

        unsigned int VAO, VBO, EBO;
        glm::vec4 UV; // NEW: Store UV coordinates

        void UpdateVertexData(); // NEW: Update VBO with new UV
    };
};

#endif
