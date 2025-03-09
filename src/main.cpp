#define STB_IMAGE_IMPLEMENTATION

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <sstream>
#include "glm/glm.hpp"

#include "util/util.h"
#include "rendering/render_target/render_target.h"
#include "rendering/camera/camera.h"
#include "rendering/sprites/sprite.h"
#include "rendering/materials/material.h"
#include "rendering/model/model.h"
#include "rendering/text/text.h"

unsigned int WindowWidth = 800;
unsigned int WindowHeight = 600;

struct PointLight
{
    glm::vec3 Position;
    glm::vec3 Color;
    float Intensity;
};
std::vector<PointLight> pointLights = {
};

struct DirectionalLight
{
    glm::vec3 Direction;
    glm::vec3 Color;
    float Intensity;
};
std::vector<DirectionalLight> directionalLights = {
    {glm::vec3(1.0f, -1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), 2.0f}
};

struct Spotlight
{
    glm::vec3 Position;
    glm::vec3 Direction;
    glm::vec3 Color;
    float Intensity;
    float CutOff;
    float OuterCutOff;
};
std::vector<Spotlight> spotlights = {

};


Engine::Camera MainCamera(Engine::Camera::CameraMode::Perspective, &WindowWidth, &WindowHeight);

Engine::RenderTarget *SceneRenderTarget;
Engine::Sprite *RenderTargetSprite;
Engine::Material *RenderTargetMaterial;

Engine::Material *SpriteMaterial;
Engine::Sprite *TestSprite;

Engine::Model::ModelInstance *Model;

Engine::Material *FontMaterial;
Engine::Text *UIText;

float LastTime = 0.0f;  // Store the time of the last frame
float DeltaTime = 0.0f; // Time between frames
float FPS = 0.0f;       // Frames per second

void InitRenderTarget()
{
    std::vector<Engine::RenderTarget::Attachment> Attachments = {
        {GL_RGBA16F, GL_RGBA, GL_UNSIGNED_BYTE}, // Albedo
        {GL_RGB16F, GL_RGB, GL_FLOAT},           // Normal
        {GL_RGB16F, GL_RGB, GL_FLOAT},           // Position
        {GL_R32F, GL_RED, GL_FLOAT},             // Depth
        {GL_R16F, GL_RED, GL_FLOAT},             // Metallic
        {GL_R16F, GL_RED, GL_FLOAT},             // Roughness
        {GL_RGB16F, GL_RGB, GL_FLOAT}            // Emission
    };
    SceneRenderTarget = new Engine::RenderTarget(glm::vec2(WindowWidth, WindowHeight), Attachments);
    RenderTargetMaterial = new Engine::Material("assets/shaders/deferred/vert.glsl",
                                                "assets/shaders/deferred/lighting.glsl",
                                                {});
    RenderTargetSprite = new Engine::Sprite(RenderTargetMaterial, glm::vec2(0, 0), glm::vec2(0, 0), &WindowWidth, &WindowHeight);
}

void InitText()
{
    FontMaterial = new Engine::Material("assets/shaders/main/vert.glsl", "assets/shaders/main/frag.glsl", {"assets/textures/font/arial.png"});
    FontMaterial->SetUniform("Color", glm::vec4(0.0f, 1.0f, 1.0f, 1.0f));
    FontMaterial->SetDepthSortingMode(Engine::Material::DepthSortingMode::None);
    FontMaterial->SetBlendingMode(Engine::Material::BlendingMode::AlphaBlend);
    UIText = new Engine::Text(FontMaterial, glm::vec2(0, 0), 32.0f, &WindowWidth, &WindowHeight);
}

void RenderText(const std::string &text)
{
    float ScaleFactor = (WindowWidth < WindowHeight ? WindowWidth : WindowHeight) / 10.0f;
    UIText->SetPosition(glm::vec2(ScaleFactor / 2, ScaleFactor / 2));
    UIText->SetScale(ScaleFactor / 3);
    UIText->Render(text.c_str());
}

void InitModel()
{
    Engine::Model::Mesh Mesh = Engine::Model::LoadMesh("assets/models/sponza.obj");

    std::vector<Engine::Material*> AssignedMaterials(Mesh.MaterialData.size(), nullptr);

    for (unsigned int i = 0; i < Mesh.Lights.size(); i++) {
        const Engine::Model::LightData& light = Mesh.Lights[i];
        
        switch (light.Type) {
            case Engine::Model::LightData::LightType::POINT:
                pointLights.push_back(PointLight{
                    light.Position,
                    light.Color,
                    light.Intensity
                });
                break;
            
            case Engine::Model::LightData::LightType::DIRECTIONAL:
                directionalLights.push_back(DirectionalLight{
                    light.Direction,
                    light.Color,
                    light.Intensity
                });
                break;
            
            case Engine::Model::LightData::LightType::SPOT:
                spotlights.push_back(Spotlight{
                    light.Position,
                    light.Direction,
                    light.Color,
                    light.Intensity,
                    light.InnerCone,
                    light.OuterCone
                });
                break;
            
            case Engine::Model::LightData::LightType::AMBIENT:
                break;
            
            default:
                break;
        }
    }
    

    for (unsigned int i = 0; i < Mesh.MaterialData.size(); i++)
    {
        Engine::Model::MaterialData& Data = Mesh.MaterialData[i];

        Engine::Material* NewMaterial = new Engine::Material(
            "assets/shaders/deferred/vert.glsl",
            "assets/shaders/deferred/frag.glsl"
        );

        NewMaterial->SetUniform("Color", Data.DiffuseColor);

        if (!Data.DiffuseTextures.empty()) {
            NewMaterial->LoadTexture(0, "assets/models/" + Data.DiffuseTextures[0]);
            NewMaterial->SetUniform("UseTexture", true);
        }
        if (!Data.NormalTextures.empty()) {
            NewMaterial->LoadTexture(1, "assets/models/" + Data.NormalTextures[0]);
            NewMaterial->SetUniform("UseNormalMap", true);
        }

        AssignedMaterials[i] = NewMaterial;
    }

    std::vector<Engine::Material*> MeshMaterials;
    
    for (size_t i = 0; i < Mesh.Meshes.size(); i++)
    {
        unsigned int MaterialIndex = Mesh.Meshes[i].MaterialIndex;
        MeshMaterials.push_back(AssignedMaterials[MaterialIndex]);
    }

    Model = new Engine::Model::ModelInstance(
        Mesh,
        MeshMaterials,
        glm::mat4(1.0f)
    );
}

void RenderModel()
{
    glm::mat4 ModelMatrix = glm::mat4(1.0f);
    ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, -50.0f, 0.0f));
    ModelMatrix = glm::scale(ModelMatrix, glm::vec3(0.25f, 0.25f,0.25f));
    Model->Transform = ModelMatrix;

    Engine::Model::DrawModelInstances({*Model}, &MainCamera);
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
    glm::mat4 RotationMatrix = glm::rotate(glm::mat4(1.0f), (glm::mediump_float32)glm::radians(glfwGetTime() * 10.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    MainCamera.SetRotation(RotationMatrix);

    glfwPollEvents();

    SceneRenderTarget->Resize(glm::vec2(WindowWidth, WindowHeight));
    SceneRenderTarget->Bind();

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    RenderModel();

    SceneRenderTarget->Unbind();

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    RenderTargetSprite->GetMaterial()->SetTexture(0, SceneRenderTarget->Textures[0]); // Albedo
    RenderTargetSprite->GetMaterial()->SetTexture(1, SceneRenderTarget->Textures[1]); // Normal
    RenderTargetSprite->GetMaterial()->SetTexture(2, SceneRenderTarget->Textures[2]); // Position
    RenderTargetSprite->GetMaterial()->SetTexture(3, SceneRenderTarget->Textures[4]); // Metallic
    RenderTargetSprite->GetMaterial()->SetTexture(4, SceneRenderTarget->Textures[5]); // Smoothness
    RenderTargetSprite->GetMaterial()->SetTexture(5, SceneRenderTarget->Textures[6]); // Emission

    // Pass all the textures to the shader
    RenderTargetSprite->GetMaterial()->SetUniform("NormalTexture", 1);
    RenderTargetSprite->GetMaterial()->SetUniform("PositionTexture", 2);
    RenderTargetSprite->GetMaterial()->SetUniform("MetallicTexture", 3);
    RenderTargetSprite->GetMaterial()->SetUniform("RoughnessTexture", 4);
    RenderTargetSprite->GetMaterial()->SetUniform("EmissionTexture", 5);

    RenderTargetSprite->GetMaterial()->SetUniform("ViewPosition", MainCamera.GetPosition());

    // Set Point Lights
    RenderTargetSprite->GetMaterial()->SetUniform("NumPointLights", (int)pointLights.size());
    for (size_t i = 0; i < pointLights.size(); i++)
    {
        std::string index = std::to_string(i);
        RenderTargetSprite->GetMaterial()->SetUniform(("PointLights[" + index + "].Position").c_str(), pointLights[i].Position);
        RenderTargetSprite->GetMaterial()->SetUniform(("PointLights[" + index + "].Color").c_str(), pointLights[i].Color);
        RenderTargetSprite->GetMaterial()->SetUniform(("PointLights[" + index + "].Intensity").c_str(), pointLights[i].Intensity);
    }

    // Set Directional Lights
    RenderTargetSprite->GetMaterial()->SetUniform("NumDirectionalLights", (int)directionalLights.size());
    for (size_t i = 0; i < directionalLights.size(); i++)
    {
        std::string index = std::to_string(i);
        RenderTargetSprite->GetMaterial()->SetUniform(("DirectionalLights[" + index + "].Direction").c_str(), directionalLights[i].Direction);
        RenderTargetSprite->GetMaterial()->SetUniform(("DirectionalLights[" + index + "].Color").c_str(), directionalLights[i].Color);
        RenderTargetSprite->GetMaterial()->SetUniform(("DirectionalLights[" + index + "].Intensity").c_str(), directionalLights[i].Intensity);
    }

    RenderTargetSprite->GetMaterial()->SetUniform("NumSpotLights", (int)spotlights.size());
    for (size_t i = 0; i < spotlights.size(); i++)
    {
        std::string index = std::to_string(i);
        RenderTargetSprite->GetMaterial()->SetUniform(("SpotLights[" + index + "].Position").c_str(), spotlights[i].Position);
        RenderTargetSprite->GetMaterial()->SetUniform(("SpotLights[" + index + "].Direction").c_str(), spotlights[i].Direction);
        RenderTargetSprite->GetMaterial()->SetUniform(("SpotLights[" + index + "].Color").c_str(), spotlights[i].Color);
        RenderTargetSprite->GetMaterial()->SetUniform(("SpotLights[" + index + "].Intensity").c_str(), spotlights[i].Intensity);
        RenderTargetSprite->GetMaterial()->SetUniform(("SpotLights[" + index + "].Cutoff").c_str(), spotlights[i].CutOff);
        RenderTargetSprite->GetMaterial()->SetUniform(("SpotLights[" + index + "].OuterCutoff").c_str(), spotlights[i].OuterCutOff);
    }

    RenderTargetSprite->SetSize(glm::vec2(WindowWidth, WindowHeight));
    RenderTargetSprite->Render();

    // FPS Calculation and Display
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

    GLFWwindow *Window = glfwCreateWindow(WindowWidth, WindowHeight, "SPONZAAA!!!", nullptr, nullptr);
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
    InitModel();
    MainCamera.SetPosition(glm::vec3(0, 0, 1));

    while (!glfwWindowShouldClose(Window))
    {
        Render(Window);
    }

    for(unsigned int i = 0; i < Model->Materials.size(); i++){
        delete Model->Materials[i];
    }

    Engine::Model::UnloadModelInstance(*Model);

    delete FontMaterial;
    delete RenderTargetMaterial;

    delete UIText;
    delete RenderTargetSprite;

    delete SceneRenderTarget;

    glfwDestroyWindow(Window);
    glfwTerminate();

    return 0;
}
