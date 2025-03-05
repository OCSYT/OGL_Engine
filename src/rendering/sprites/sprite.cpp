#include "sprite.h"

Engine::Sprite::Sprite(Material* material, const glm::vec2 &position,
               const glm::vec2 &size, unsigned int *screenWidth, unsigned int *screenHeight)
    : Position(position), Size(size),
      ScreenWidth(screenWidth), ScreenHeight(screenHeight)
{
    MaterialInstance = material;
    MaterialInstance->SetUniform("UseTexture", true);

    // Default full UV range (entire texture)
    UV = glm::vec4(0.0f, 1.0f, 1.0f, 0.0f); 

    UpdateVertexData();
}

Engine::Sprite::~Sprite()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void Engine::Sprite::UpdateVertexData()
{
    // Delete old buffers and VAO to avoid memory leaks
    if (VAO != 0)
        glDeleteVertexArrays(1, &VAO);
    if (VBO != 0)
        glDeleteBuffers(1, &VBO);
    if (EBO != 0)
        glDeleteBuffers(1, &EBO);

    // Generate new VAO, VBO, and EBO
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    float Vertices[] = {
        // Position        // UV        // Normal      // Color
         1.0f,  0.0f, 0.0f,  UV.z, UV.y,   0.0f, 0.0f, 1.0f,   1.0f, 1.0f, 1.0f, // Top-right
         1.0f,  1.0f, 0.0f,  UV.z, UV.w,   0.0f, 0.0f, 1.0f,   1.0f, 1.0f, 1.0f, // Bottom-right
         0.0f,  1.0f, 0.0f,  UV.x, UV.w,   0.0f, 0.0f, 1.0f,   1.0f, 1.0f, 1.0f, // Bottom-left
         0.0f,  0.0f, 0.0f,  UV.x, UV.y,   0.0f, 0.0f, 1.0f,   1.0f, 1.0f, 1.0f  // Top-left
    };

    unsigned int Indices[] = { 0, 1, 3, 1, 2, 3 };

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void *)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void *)(8 * sizeof(float))); 
    glEnableVertexAttribArray(3);

    glBindVertexArray(0);
}


void Engine::Sprite::Render()
{
    if (*ScreenHeight == 0)
        return; 

    GLboolean depthEnabled;
    glGetBooleanv(GL_DEPTH_TEST, &depthEnabled);
    glDisable(GL_DEPTH_TEST);

    glm::mat4 view = glm::mat4(1);
    glm::mat4 projection = glm::ortho(
        0.0f, static_cast<float>(*ScreenWidth),
        static_cast<float>(*ScreenHeight), 0.0f,
        -1.0f, 1.0f
    );

    MaterialInstance->Bind();

    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(Position, 0.0f));
    model = glm::scale(model, glm::vec3(Size, 1.0f));

    MaterialInstance->SetUniform("Model", model);
    MaterialInstance->SetUniform("View", view);
    MaterialInstance->SetUniform("Projection", projection);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    if (depthEnabled)
        glEnable(GL_DEPTH_TEST);
}

void Engine::Sprite::SetPosition(const glm::vec2 &position)
{
    Position = position;
}

void Engine::Sprite::SetSize(const glm::vec2 &size)
{
    Size = size;
}

void Engine::Sprite::SetMaterial(Material* material)
{
    MaterialInstance = material;
    MaterialInstance->SetUniform("UseTexture", true);
}

void Engine::Sprite::SetUV(const glm::vec4 &uv)
{
    if (UV != uv) {
        UV = uv;
        UpdateVertexData();
    }
}

Engine::Material* Engine::Sprite::GetMaterial() const
{
    return MaterialInstance;
}

glm::vec2 Engine::Sprite::GetPosition() const
{
    return Position;
}

glm::vec2 Engine::Sprite::GetSize() const
{
    return Size;
}
