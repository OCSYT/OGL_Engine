#include "sprite.h"

Sprite::Sprite(const Material &material, const glm::vec2 &position,
               const glm::vec2 &size, Anchor anchor, float screenWidth, float screenHeight)
    : MaterialInstance(material), Position(position), Size(size), AnchorPoint(anchor),
      ScreenWidth(screenWidth), ScreenHeight(screenHeight)
{

    // Initialize vertex data
    float Vertices[] = {
        0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,   // top right
        0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, // bottom left
        -0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f   // top left
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

void Sprite::Render(const glm::mat4 &view, const glm::mat4 &projection)
{
    MaterialInstance.Bind();
    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(Position, 0.0f));
    model = glm::scale(model, glm::vec3(Size, 1.0f));
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
    CalculateAnchorOffset();
}

void Sprite::SetSize(const glm::vec2 &size)
{
    Size = size;
}

void Sprite::SetAnchor(Anchor anchor)
{
    AnchorPoint = anchor;
    CalculateAnchorOffset();
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

void Sprite::CalculateAnchorOffset()
{
    switch (AnchorPoint)
    {
    case Anchor::TopLeft:
        AnchorOffset = glm::vec2(0.0f, 1.0f); // Top-left corner
        break;
    case Anchor::TopCenter:
        AnchorOffset = glm::vec2(0.5f, 1.0f); // Top-center
        break;
    case Anchor::TopRight:
        AnchorOffset = glm::vec2(1.0f, 1.0f); // Top-right corner
        break;
    case Anchor::CenterLeft:
        AnchorOffset = glm::vec2(0.0f, 0.5f); // Center-left
        break;
    case Anchor::Center:
        AnchorOffset = glm::vec2(0.5f, 0.5f); // Center
        break;
    case Anchor::CenterRight:
        AnchorOffset = glm::vec2(1.0f, 0.5f); // Center-right
        break;
    case Anchor::BottomLeft:
        AnchorOffset = glm::vec2(0.0f, 0.0f); // Bottom-left corner
        break;
    case Anchor::BottomCenter:
        AnchorOffset = glm::vec2(0.5f, 0.0f); // Bottom-center
        break;
    case Anchor::BottomRight:
        AnchorOffset = glm::vec2(1.0f, 0.0f); // Bottom-right corner
        break;
    }

    // Adjust Position based on screen size and anchor
    Position.x = (Position.x / ScreenWidth) * 2.0f - 1.0f;  // Normalize x position to [-1, 1]
    Position.y = (Position.y / ScreenHeight) * 2.0f - 1.0f; // Normalize y position to [-1, 1]

    // Adjust the final position based on the anchor point
    Position -= Size * AnchorOffset;
}
