#include "sprite.h"

Sprite::Sprite(const Material &material, const glm::vec2 &position,
               const glm::vec2 &size, unsigned int *screenWidth, unsigned int *screenHeight)
    : MaterialInstance(material), Position(position), Size(size),
      ScreenWidth(screenWidth), ScreenHeight(screenHeight)
{

    // Initialize vertex data
    float Vertices[] = {
        //Pos             //Uv        //Color
        1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, // Top-right
        1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, // Bottom-right
        0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, // Bottom-left
        0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f  // Top-left
    };

    unsigned int Indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };

    // Create and bind VAO, VBO, and EBO
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
}

Sprite::~Sprite()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void Sprite::Render() {
    if (*ScreenHeight == 0) return; // Prevent division by zero

    glm::mat4 view = glm::mat4(1);

    // Orthographic projection using screen-space coordinates
    glm::mat4 projection = glm::ortho(
        0.0f, static_cast<float>(*ScreenWidth),  // Left, Right (Screen-space)
        static_cast<float>(*ScreenHeight), 0.0f, // Bottom, Top (Flipped Y to match OpenGL)
        -1.0f, 1.0f                              // Near, Far
    );

    MaterialInstance.Bind();

    // Corrected: Position should not be adjusted by size
    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(Position, 0.0f));
    model = glm::scale(model, glm::vec3(Size, 1.0f)); // Scale to pixel size

    MaterialInstance.SetUniform("Model", model);
    MaterialInstance.SetUniform("View", view);
    MaterialInstance.SetUniform("Projection", projection);
    MaterialInstance.SetUniform("UseTexture", true);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void Sprite::SetPosition(const glm::vec2 &position)
{
    Position = position;
}

void Sprite::SetSize(const glm::vec2 &size)
{
    Size = size;
}

void Sprite::SetMaterial(const Material &material)
{
    MaterialInstance = material; // Swap material
}

Material Sprite::GetMaterial() const
{
    return MaterialInstance;
}

glm::vec2 Sprite::GetPosition() const
{
    return Position;
}

glm::vec2 Sprite::GetSize() const
{
    return Size;
}
