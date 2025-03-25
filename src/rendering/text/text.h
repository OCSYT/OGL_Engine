#pragma once

#ifndef text_h
#define text_h
#define NOMINMAX
#include <algorithm>
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <iostream>
#include "../sprites/sprite.h"
#include "../materials/material.h"
#include <unordered_map>

namespace Engine
{
    class Text
    {
    public:
        enum class TextAlign { Left, Center, Right };
        Sprite *SpriteRenderer;
        Text(Material *material, const glm::vec2 &position, float scale, unsigned int *screenWidth, unsigned int *screenHeight);
        ~Text();

        void Render(const std::string &text, TextAlign alignment);
        void SetCharacterMap();
        void SetPosition(const glm::vec2 &position);
        void SetScale(float scale);
        void SetSpacing(float Spacing);
        void SetLineheight(float Lineheight);
        glm::vec2 GetPosition() const;
        float GetScale() const;
        float GetSpacing() const;
        float GetLineheight() const;

    private:
        Material *MaterialInstance;
        glm::vec2 Position;
        float Scale = 32;
        float Spacing = 1;
        float Lineheight = 1;
        unsigned int *ScreenWidth;
        unsigned int *ScreenHeight;

        std::unordered_map<std::string, int> CharacterMap;
        std::vector<glm::vec4> CharUVs;

        void SetCharacterUVs();
        void CreateSprites();
    };
};

#endif
