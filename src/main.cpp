#define STB_IMAGE_IMPLEMENTATION

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <sstream>
#include <glm/glm.hpp>

#include "util/util.h"
#include "rendering/render_target/render_target.h"
#include "rendering/camera/camera.h"
#include "rendering/sprites/sprite.h"
#include "rendering/materials/material.h"
#include "rendering/model/model.h"
#include "rendering/text/text.h"

unsigned int WindowWidth = 800;
unsigned int WindowHeight = 600;

Engine::Camera MainCamera(Engine::Camera::CameraMode::Perspective, &WindowWidth, &WindowHeight);

RenderTarget *SceneRenderTarget;
Engine::Sprite *RenderTargetSprite;
Engine::Material *RenderTargetMaterial;

Engine::Material *SpriteMaterial;
Engine::Sprite *TestSprite;

Engine::Model::Mesh MarkiplierModel;
Engine::Material *Skin;
Engine::Material *Eye;
Engine::Material *Glasses;
Engine::Material *Hair;

Engine::Material *FontMaterial;
Engine::Text *UIText;

float LastTime = 0.0f;  // Store the time of the last frame
float DeltaTime = 0.0f; // Time between frames
float FPS = 0.0f;       // Frames per second

void InitRenderTarget()
{
    SceneRenderTarget = new RenderTarget(glm::vec2(WindowWidth, WindowHeight));
    RenderTargetMaterial = new Engine::Material("assets/shaders/main/vert.glsl",
                                                "assets/shaders/main/frag.glsl",
                                                {});
    RenderTargetSprite = new Engine::Sprite(RenderTargetMaterial, glm::vec2(0,0), glm::vec2(0, 0), &WindowWidth, &WindowHeight);
}

void InitText()
{
    FontMaterial = new Engine::Material("assets/shaders/main/vert.glsl", "assets/shaders/main/frag.glsl", {"assets/textures/font/arial.png"});
    FontMaterial->SetUniform("Color", glm::vec4(0.0f, 1.0f, 1.0f, 1.0f));
    UIText = new Engine::Text(FontMaterial, glm::vec2(0, 0), 32.0f, &WindowWidth, &WindowHeight);
}

void RenderText(const std::string &text)
{
    float ScaleFactor = (WindowWidth < WindowHeight ? WindowWidth : WindowHeight) / 10.0f;
    UIText->SetPosition(glm::vec2(ScaleFactor / 2, ScaleFactor / 2));
    UIText->SetScale(ScaleFactor / 3);
    UIText->Render(text.c_str());
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

void CalculateFPS()
{
    float currentTime = glfwGetTime();
    DeltaTime = currentTime - LastTime;
    LastTime = currentTime;

    if (DeltaTime > 0.0f)
    {
        FPS = 1.0f / DeltaTime; // FPS = 1 / DeltaTime
    }
}

void Render(GLFWwindow *Window)
{
    glfwPollEvents();

    SceneRenderTarget->Resize(glm::vec2(WindowWidth, WindowHeight));
    SceneRenderTarget->Bind();

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f); // Set background color
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    RenderMarkiplier();

    SceneRenderTarget->Unbind();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    RenderTargetSprite->GetMaterial()->SetTexture(0, SceneRenderTarget->Texture);
    RenderTargetSprite->SetSize(glm::vec2(WindowWidth, WindowHeight));
    RenderTargetSprite->Render();

    CalculateFPS();
    std::stringstream ss;
    ss << "FPS: " << static_cast<int>(FPS);
    RenderText(ss.str());

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

    InitRenderTarget();
    InitText();
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
    delete FontMaterial;
    delete RenderTargetMaterial;

    delete UIText;
    delete RenderTargetSprite;

    delete SceneRenderTarget;


    glfwDestroyWindow(Window);
    glfwTerminate();

    return 0;
}
