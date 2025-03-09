#include "text.h"

namespace Engine
{
    Text::Text(Material *material, const glm::vec2 &position, float scale, unsigned int *screenWidth, unsigned int *screenHeight)
        : MaterialInstance(material), Position(position), Scale(scale), ScreenWidth(screenWidth), ScreenHeight(screenHeight)
    {
        SetCharacterMap();
        SetCharacterUVs();
        SpriteRenderer = new Sprite(MaterialInstance, Position, glm::vec2(Scale, Scale), ScreenWidth, ScreenHeight);
    }

    Text::~Text()
    {
        delete SpriteRenderer;
    }
    void Text::SetCharacterMap()
    {
        CharacterMap[" "] = 0;
        CharacterMap["!"] = 1;
        CharacterMap["\""] = 2;
        CharacterMap["#"] = 3;
        CharacterMap["$"] = 4;
        CharacterMap["%"] = 5;
        CharacterMap["&"] = 6;
        CharacterMap["'"] = 7;
        CharacterMap["("] = 8;
        CharacterMap[")"] = 9;
        CharacterMap["*"] = 10;
        CharacterMap["+"] = 11;
        CharacterMap[","] = 12;
        CharacterMap["-"] = 13;
        CharacterMap["."] = 14;
        CharacterMap["/"] = 15;
        CharacterMap["0"] = 16;
        CharacterMap["1"] = 17;
        CharacterMap["2"] = 18;
        CharacterMap["3"] = 19;
        CharacterMap["4"] = 20;
        CharacterMap["5"] = 21;
        CharacterMap["6"] = 22;
        CharacterMap["7"] = 23;
        CharacterMap["8"] = 24;
        CharacterMap["9"] = 25;
        CharacterMap[":"] = 26;
        CharacterMap[";"] = 27;
        CharacterMap["<"] = 28;
        CharacterMap["="] = 29;
        CharacterMap[">"] = 30;
        CharacterMap["?"] = 31;
        CharacterMap["@"] = 32;
        CharacterMap["A"] = 33;
        CharacterMap["B"] = 34;
        CharacterMap["C"] = 35;
        CharacterMap["D"] = 36;
        CharacterMap["E"] = 37;
        CharacterMap["F"] = 38;
        CharacterMap["G"] = 39;
        CharacterMap["H"] = 40;
        CharacterMap["I"] = 41;
        CharacterMap["J"] = 42;
        CharacterMap["K"] = 43;
        CharacterMap["L"] = 44;
        CharacterMap["M"] = 45;
        CharacterMap["N"] = 46;
        CharacterMap["O"] = 47;
        CharacterMap["P"] = 48;
        CharacterMap["Q"] = 49;
        CharacterMap["R"] = 50;
        CharacterMap["S"] = 51;
        CharacterMap["T"] = 52;
        CharacterMap["U"] = 53;
        CharacterMap["V"] = 54;
        CharacterMap["W"] = 55;
        CharacterMap["X"] = 56;
        CharacterMap["Y"] = 57;
        CharacterMap["Z"] = 58;
        CharacterMap["["] = 59;
        CharacterMap["\\"] = 60;
        CharacterMap["]"] = 61;
        CharacterMap["^"] = 62;
        CharacterMap["_"] = 63;
        CharacterMap["`"] = 64;
        CharacterMap["a"] = 65;
        CharacterMap["b"] = 66;
        CharacterMap["c"] = 67;
        CharacterMap["d"] = 68;
        CharacterMap["e"] = 69;
        CharacterMap["f"] = 70;
        CharacterMap["g"] = 71;
        CharacterMap["h"] = 72;
        CharacterMap["i"] = 73;
        CharacterMap["j"] = 74;
        CharacterMap["k"] = 75;
        CharacterMap["l"] = 76;
        CharacterMap["m"] = 77;
        CharacterMap["n"] = 78;
        CharacterMap["o"] = 79;
        CharacterMap["p"] = 80;
        CharacterMap["q"] = 81;
        CharacterMap["r"] = 82;
        CharacterMap["s"] = 83;
        CharacterMap["t"] = 84;
        CharacterMap["u"] = 85;
        CharacterMap["v"] = 86;
        CharacterMap["w"] = 87;
        CharacterMap["x"] = 88;
        CharacterMap["y"] = 89;
        CharacterMap["z"] = 90;
        CharacterMap["{"] = 91;
        CharacterMap["|"] = 92;
        CharacterMap["}"] = 93;
        CharacterMap["~"] = 94;
    }
    

    void Text::SetCharacterUVs()
    {
        int gridSize = 10;
        float charSize = 1.0f / gridSize;

        CharUVs.resize(100);

        for (int i = 0; i < 100; ++i)
        {
            int row = (gridSize) - (i / gridSize);
            int col = i % gridSize;

            float xStart = col * charSize;
            float yStart = row * charSize;
            float xEnd = xStart + charSize;  
            float yEnd = yStart - charSize; 

            CharUVs[i] = glm::vec4(xStart, yStart, xEnd, yEnd);
        }
    }

    void Text::Render(const std::string &text)
    {
        float xOffset = 0.0f;
        float yOffset = 0.0f;
        float Spacing = Scale;
        float LineHeight = Scale;
        for (char c : text)
        {
            if (c == ' ')
            {
                xOffset += Spacing;
                continue;
            }
    
            if (c == '\n')
            {
                xOffset = 0.0f;
                yOffset -= LineHeight;
                continue;
            }
    
            auto it = CharacterMap.find(std::string(1, c));
            if (it == CharacterMap.end())
            {
                std::cout << "Warning: Character '" << c << "' not found in the CharacterMap!" << std::endl;
                c = '?';
            }
    
            glm::vec4 uv = CharUVs[CharacterMap[std::string(1, c)]];
            SpriteRenderer->SetSize(glm::vec2(Scale, Scale));
            SpriteRenderer->SetPosition(Position + glm::vec2(xOffset, yOffset));
            SpriteRenderer->SetUV(uv);
            xOffset += Spacing;
            SpriteRenderer->Render();
        }
    }
    

    void Text::SetPosition(const glm::vec2 &position)
    {
        Position = position;
    }

    void Text::SetScale(float scale)
    {
        Scale = scale;
    }
}
