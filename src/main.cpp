
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "util/util.h"
#include <filesystem>
#include "rendering/shaders/shader.h"
#include "rendering/camera/camera.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

const unsigned int WindowWidth = 800;
const unsigned int WindowHeight = 600;
Camera MainCamera(CameraMode::Perspective, (float)WindowWidth / (float)WindowHeight);

const float Vertices[] = {
    // positions          // colors           // texture coords
    0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
    0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
    -0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
    -0.5f,  0.5f, 0.0f,  1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left 
};
unsigned int Indices[] = {  
    0, 1, 3, // first triangle
    1, 2, 3  // second triangle
};

unsigned int LoadTexture(const char *Path)
{
    unsigned int TextureID;
    glGenTextures(1, &TextureID);
    glBindTexture(GL_TEXTURE_2D, TextureID);

    // Set texture wrapping/filtering options
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Load image data
    int Width, Height, NumChannels;
    unsigned char *Data = stbi_load(Path, &Width, &Height, &NumChannels, 0);
    if (Data)
    {
        GLenum Format = (NumChannels == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, Format, Width, Height, 0, Format, GL_UNSIGNED_BYTE, Data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cerr << "Failed to load texture: " << Path << std::endl;
    }

    stbi_image_free(Data);
    return TextureID;
}

void FramebufferSizeCallback(GLFWwindow *Window, int Width, int Height)
{
    float WindowAspectRatio = (float)WindowWidth / (float)WindowHeight;
    float CurrentAspectRatio = (float)Width / (float)Height;

    int NewWidth, NewHeight, XOffset, YOffset;

    if (CurrentAspectRatio > WindowAspectRatio)
    {
        // Window is wider than the intended aspect ratio
        NewWidth = (int)(Height * WindowAspectRatio);
        NewHeight = Height;
        XOffset = (Width - NewWidth) / 2;
        YOffset = 0;
    }
    else
    {
        // Window is taller than the intended aspect ratio
        NewWidth = Width;
        NewHeight = (int)(Width / WindowAspectRatio);
        XOffset = 0;
        YOffset = (Height - NewHeight) / 2;
    }

    glViewport(XOffset, YOffset, NewWidth, NewHeight);
}

int main()
{
    MainCamera.SetPosition(glm::vec3(0.0f, 0.0f, 3.0f));
    MainCamera.SetPerspective(60.0f, 0.1f, 100.0f);

    if (!glfwInit())
    {
        std::cerr << "Could not initialize GLFW" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *Window = glfwCreateWindow(WindowWidth, WindowHeight, "Unlit Rendering", nullptr, nullptr);
    if (!Window)
    {
        std::cerr << "Could not create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(Window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Could not initialize GLAD" << std::endl;
        return -1;
    }

    // Callbacks
    glfwSetFramebufferSizeCallback(Window, FramebufferSizeCallback);

    unsigned int VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);

    // Position attribute (Location 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    // Color attribute (Location 1)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Texture coordinate attribute (Location 2)
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);


    Shader ShaderProgram("shaders/main/vert.glsl", "shaders/main/frag.glsl");
    ShaderProgram.Bind();

    glm::mat4 View = MainCamera.GetViewMatrix();
    glm::mat4 Projection = MainCamera.GetProjectionMatrix();
    glm::mat4 Model = glm::mat4(1.0f);
    Model = glm::translate(Model, glm::vec3(0.0f, 0.0f, 0.0f));

    ShaderProgram.SetUniform("View", View);
    ShaderProgram.SetUniform("Projection", Projection);
    ShaderProgram.SetUniform("Model", Model);
    ShaderProgram.SetUniform("UseTexture", true);
    // ShaderProgram.SetUniform("Texture", 0);

    std::filesystem::path TexturePath = std::filesystem::path(Util::GetExecutablePath()) / "test.png";
    std::string TexturePathStr = TexturePath.string();
    const char *Path = TexturePathStr.c_str();

    unsigned int TextureID = LoadTexture(Path);

    glClearColor(0, 0, 0, 1.0f);

    while (!glfwWindowShouldClose(Window))
    {
        glfwPollEvents();

        glClear(GL_COLOR_BUFFER_BIT);

        ShaderProgram.Bind();

        // Update View and Projection matrices every frame (important if the camera moves)
        View = MainCamera.GetViewMatrix();
        Projection = MainCamera.GetProjectionMatrix();
        ShaderProgram.SetUniform("View", View);
        ShaderProgram.SetUniform("Projection", Projection);
        ShaderProgram.SetUniform("Model", Model);
        ShaderProgram.SetUniform("UseTexture", true);

        glBindTexture(GL_TEXTURE_2D, TextureID);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(Window);
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    glfwDestroyWindow(Window);
    glfwTerminate();
    return 0;
}
