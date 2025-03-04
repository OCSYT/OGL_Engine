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

Engine::Model::Mesh MarkiplierModel;
Engine::Material *Skin;
Engine::Material *Eye;
Engine::Material *Glasses;
Engine::Material *Hair;

void FramebufferSizeCallback(GLFWwindow *Window, int Width, int Height)
{
    if (Height == 0)
        return;
    WindowWidth = Width;
    WindowHeight = Height;
    glViewport(0, 0, WindowWidth, WindowHeight);
}

void InitMarkiplier()
{
    MarkiplierModel = Engine::Model::LoadModel("assets/models/Markiplier.obj");

    Skin = new Engine::Material("assets/shaders/main/vert.glsl",
                                "assets/shaders/main/frag.glsl",
                                {"assets/textures/Markiplier/Skin.png"});

    Eye = new Engine::Material("assets/shaders/main/vert.glsl",
                               "assets/shaders/main/frag.glsl",
                               {"assets/textures/Markiplier/Eye.png"});

    Glasses = new Engine::Material("assets/shaders/main/vert.glsl",
                                   "assets/shaders/main/frag.glsl",
                                   {});

    Hair = new Engine::Material("assets/shaders/main/vert.glsl",
                               "assets/shaders/main/frag.glsl",
                               {"assets/textures/Markiplier/Hair.png"});
}

void RenderMarkiplier()
{

    glm::mat4 ModelMatrix = glm::rotate(glm::mat4(1.0f), static_cast<float>(glfwGetTime()), glm::vec3(0.0f, 1.0f, 0.0f));
    ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, -0.25f, 0.0f));

    Engine::Model::DrawModel(MarkiplierModel.Meshes[0], Skin, ModelMatrix, &MainCamera);
    Skin->SetUniform("UseTexture", true);
    Engine::Model::DrawModel(MarkiplierModel.Meshes[1], Eye, ModelMatrix, &MainCamera);
    Eye->SetUniform("UseTexture", true);
    Engine::Model::DrawModel(MarkiplierModel.Meshes[2], Glasses, ModelMatrix, &MainCamera);
    Glasses->SetUniform("UseTexture", false);
    Engine::Model::DrawModel(MarkiplierModel.Meshes[3], Hair, ModelMatrix, &MainCamera);
    Hair->SetUniform("UseTexture", true);
}

int main()
{
    if (!glfwInit())
        return -1;

    GLFWwindow *Window = glfwCreateWindow(WindowWidth, WindowHeight, "MARKIPLIER!!!", nullptr, nullptr);
    if (!Window)
        return -1;

    glfwMakeContextCurrent(Window);
    glfwSetFramebufferSizeCallback(Window, FramebufferSizeCallback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        return -1;

    InitMarkiplier();
    Engine::Material SpriteMaterial("assets/shaders/main/vert.glsl", "assets/shaders/main/frag.glsl", {"assets/textures/sprite.png"});
    Engine::Sprite TestSprite(SpriteMaterial, glm::vec2(0, 0), glm::vec2(250.0f, 250.0f), &WindowWidth, &WindowHeight);

    MainCamera.SetPosition(glm::vec3(0, 0, 1));

    while (!glfwWindowShouldClose(Window))
    {
        glfwPollEvents();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        RenderMarkiplier();
        TestSprite.Render();
        TestSprite.SetPosition(glm::vec2(WindowWidth - 275.0f, 25.0f));

        glfwSwapBuffers(Window);
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
