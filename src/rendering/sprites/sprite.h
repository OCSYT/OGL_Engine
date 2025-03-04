#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "../../Rendering/Materials/Material.h"

class Sprite {
public:
    Sprite(const Material& material, const glm::vec2& position,
           const glm::vec2& size, unsigned int* screenWidth, unsigned int* screenHeight);
    ~Sprite();

    void Render();

    void SetPosition(const glm::vec2& position);
    void SetSize(const glm::vec2& size);
    void SetMaterial(const Material& material);

    Material GetMaterial() const;
    glm::vec2 GetPosition() const;
    glm::vec2 GetSize() const;

private:
    Material MaterialInstance;
    glm::vec2 Position;
    glm::vec2 Size;

    unsigned int* ScreenWidth;
    unsigned int* ScreenHeight;

    unsigned int VAO, VBO, EBO;
};
