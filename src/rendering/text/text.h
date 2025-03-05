#pragma once

#ifndef text_h
#define text_h

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
        Sprite* SpriteRenderer;
        Text(Material* material, const glm::vec2 &position, float scale, unsigned int *screenWidth, unsigned int *screenHeight);
        ~Text();

        void Render(const std::string &text);
        void SetCharacterMap(); 
        void SetPosition(const glm::vec2 &position);
        void SetScale(float scale);

    private:
    
        Material* MaterialInstance;
        glm::vec2 Position;
        float Scale;
        unsigned int *ScreenWidth;
        unsigned int *ScreenHeight;

        std::unordered_map<std::string, int> CharacterMap;
        std::vector<glm::vec4> CharUVs;

        void SetCharacterUVs();
        void CreateSprites();
    };
};

#endif
