#pragma once

#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "../../Rendering/Materials/Material.h"

class Sprite
{
public:
    enum class Anchor
    {
        TopLeft,
        TopCenter,
        TopRight,
        CenterLeft,
        Center,
        CenterRight,
        BottomLeft,
        BottomCenter,
        BottomRight
    };
    Sprite(const Material &material, const glm::vec2 &position,
           const glm::vec2 &size, Anchor anchor, float screenWidth, float screenHeight);
    ~Sprite();

    void Render(const glm::mat4 &view, const glm::mat4 &projection);

    void SetPosition(const glm::vec2 &position);
    void SetSize(const glm::vec2 &size);
    void SetAnchor(Anchor anchor);
    void SetMaterial(const Material &material);

    Material GetMaterial() const;
    glm::vec2 GetPosition() const;
    glm::vec2 GetSize() const;

private:
    void CalculateAnchorOffset();

    Material MaterialInstance;
    glm::vec2 Position;
    glm::vec2 Size;
    Anchor AnchorPoint;
    glm::vec2 AnchorOffset;

    float ScreenWidth;
    float ScreenHeight;

    unsigned int VAO, VBO, EBO;
};
