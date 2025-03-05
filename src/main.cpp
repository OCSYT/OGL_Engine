#define STB_IMAGE_IMPLEMENTATION

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "util/util.h"
#include "rendering/camera/camera.h"
#include "rendering/sprites/sprite.h"
#include "rendering/materials/material.h"
#include "rendering/model/model.h"

unsigned int WindowWidth = 800;
unsigned int WindowHeight = 600;

Engine::Camera MainCamera(Engine::Camera::CameraMode::Perspective, &WindowWidth, &WindowHeight);


Engine::Material *SpriteMaterial;
Engine::Sprite *TestSprite;

Engine::Model::Mesh MarkiplierModel;
Engine::Material *Skin;
Engine::Material *Eye;
Engine::Material *Glasses;
Engine::Material *Hair;

void InitSprite()
{
    SpriteMaterial = new Engine::Material("assets/shaders/main/vert.glsl", "assets/shaders/main/frag.glsl", {"assets/textures/sprite.png"});
    TestSprite = new Engine::Sprite(SpriteMaterial, glm::vec2(0, 0), glm::vec2(0.0f, 0.0f), &WindowWidth, &WindowHeight);
}

void InitMarkiplier()
{
    MarkiplierModel = Engine::Model::LoadMesh("assets/models/Markiplier.obj");

    Skin = new Engine::Material("assets/shaders/main/vert.glsl",
                                "assets/shaders/main/frag.glsl",
                                {"assets/textures/Markiplier/Skin.png"});
    Skin->SetUniform("UseTexture", true);

    Eye = new Engine::Material("assets/shaders/main/vert.glsl",
                               "assets/shaders/main/frag.glsl",
                               {"assets/textures/Markiplier/Eye.png"});

    Eye->SetUniform("UseTexture", true);

    Glasses = new Engine::Material("assets/shaders/main/vert.glsl",
                                   "assets/shaders/main/frag.glsl",
                                   {});

    Glasses->SetUniform("Color", glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

    Hair = new Engine::Material("assets/shaders/main/vert.glsl",
                                "assets/shaders/main/frag.glsl",
                                {"assets/textures/Markiplier/Hair.png"});

    Hair->SetUniform("UseTexture", true);
}

void RenderMarkiplier()
{
    glm::mat4 ModelMatrix = glm::rotate(glm::mat4(1.0f), static_cast<float>(glfwGetTime()), glm::vec3(0.0f, 1.0f, 0.0f));
    ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, -0.25f, 0.0f));

    Engine::Model::DrawMesh(MarkiplierModel, {Skin, Eye, Glasses, Hair}, ModelMatrix, &MainCamera);
}

void RenderSprite()
{
    float ScaleFactor = (WindowWidth < WindowHeight ? WindowWidth : WindowHeight) / 3.0f;
    TestSprite->SetSize(glm::vec2(ScaleFactor, ScaleFactor));
    TestSprite->SetPosition(glm::vec2(WindowWidth - ScaleFactor - 25.0f, 25.0f));
    TestSprite->Render();
}

void Render(GLFWwindow *Window)
{
    glfwPollEvents();

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f); // Set background color
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    RenderMarkiplier();
    RenderSprite();

    glfwSwapBuffers(Window);
}

void FramebufferSizeCallback(GLFWwindow *Window, int Width, int Height)
{
    if (Height == 0)
        return;
    WindowWidth = Width;
    WindowHeight = Height;
    glViewport(0, 0, WindowWidth, WindowHeight);
}

int main()
{
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW!" << std::endl;
        return -1;
    }

    GLFWwindow *Window = glfwCreateWindow(WindowWidth, WindowHeight, "MARKIPLIER!!!", nullptr, nullptr);
    if (!Window)
    {
        std::cerr << "Failed to create GLFW window!" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(Window);
    glfwSetFramebufferSizeCallback(Window, FramebufferSizeCallback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD!" << std::endl;
        glfwDestroyWindow(Window);
        glfwTerminate();
        return -1;
    }
    

    InitSprite();
    InitMarkiplier();
    MainCamera.SetPosition(glm::vec3(0, 0, 1));

    while (!glfwWindowShouldClose(Window))
    {
        Render(Window);
    }

    Engine::Model::UnloadMesh(MarkiplierModel);
    delete Skin;
    delete Eye;
    delete Glasses;
    delete Hair;

    glfwDestroyWindow(Window);
    glfwTerminate();

    return 0;
}
