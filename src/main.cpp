#define STB_IMAGE_IMPLEMENTATION
#define NOMINMAX
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <sstream>
#include <algorithm>

#include "util/util.h"
#include "rendering/render_target/render_target.h"
#include "rendering/camera/camera.h"
#include "rendering/sprites/sprite.h"
#include "rendering/materials/material.h"
#include "rendering/model/model.h"
#include "rendering/text/text.h"

unsigned int WindowWidth = 800, WindowHeight = 600;

struct Light
{
    glm::vec3 Position, Direction, Color;
    float Intensity, CutOff, OuterCutOff;
};
std::vector<Light> PointLights, DirectionalLights, Spotlights;

Engine::Camera MainCamera(Engine::Camera::CameraMode::Perspective, &WindowWidth, &WindowHeight);
Engine::RenderTarget *SceneRenderTarget;
Engine::Sprite *RenderTargetSprite;
Engine::Material *RenderTargetMaterial, *SpriteMaterial, *FontMaterial;
Engine::Sprite *TestSprite;
Engine::Model::ModelInstance *Model;
Engine::Text *UIText;

float LastTime = 0.0f, DeltaTime = 0.0f, FPS = 0.0f;

void InitRenderTarget()
{
    SceneRenderTarget = new Engine::RenderTarget(glm::vec2(WindowWidth, WindowHeight), {{GL_RGBA16F, GL_RGBA, GL_UNSIGNED_BYTE}, {GL_RGB16F, GL_RGB, GL_FLOAT}, {GL_RGB16F, GL_RGB, GL_FLOAT}, {GL_R32F, GL_RED, GL_FLOAT}, {GL_R16F, GL_RED, GL_FLOAT}, {GL_R16F, GL_RED, GL_FLOAT}, {GL_RGB16F, GL_RGB, GL_FLOAT}});
    RenderTargetMaterial = new Engine::Material("Assets/Shaders/Deferred/Vert.glsl", "Assets/Shaders/Deferred/Lighting.glsl", {});
    RenderTargetSprite = new Engine::Sprite(RenderTargetMaterial, glm::vec2(0, 0), glm::vec2(0, 0), &WindowWidth, &WindowHeight);
}

void InitText()
{
    FontMaterial = new Engine::Material("Assets/Shaders/Main/Vert.glsl", "Assets/Shaders/Main/Frag.glsl", {"Assets/Textures/Font/Arial.png"});
    FontMaterial->SetUniform("Color", glm::vec4(0.0f, 1.0f, 1.0f, 1.0f));
    FontMaterial->SetDepthSortingMode(Engine::Material::DepthSortingMode::None);
    FontMaterial->SetBlendingMode(Engine::Material::BlendingMode::AlphaBlend);
    UIText = new Engine::Text(FontMaterial, glm::vec2(0, 0), 32.0f, &WindowWidth, &WindowHeight);
}

void RenderText(const std::string &Text)
{
    float ScaleFactor = std::min(WindowWidth, WindowHeight) / 10.0f;
    UIText->SetPosition(glm::vec2(ScaleFactor / 2, ScaleFactor / 2));
    UIText->SetScale(ScaleFactor / 3);
    UIText->Render(Text.c_str());
}

void InitModel()
{
    DirectionalLights.push_back({{}, glm::vec3(1.0f, -1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), 2, 0, 0});
    
    Engine::Model::Mesh Mesh = Engine::Model::LoadMesh("Assets/Models/Sponza.obj");
    std::vector<Engine::Material *> AssignedMaterials(Mesh.MaterialData.size(), nullptr);

    for (const auto &Light : Mesh.Lights)
    {
        if (Light.Type == Engine::Model::LightData::LightType::Point)
            PointLights.push_back({Light.Position, {}, Light.Color, Light.Intensity, 0, 0});
        else if (Light.Type == Engine::Model::LightData::LightType::Directional)
            DirectionalLights.push_back({{}, Light.Direction, Light.Color, Light.Intensity, 0, 0});
        else if (Light.Type == Engine::Model::LightData::LightType::Spot)
            Spotlights.push_back({Light.Position, Light.Direction, Light.Color, Light.Intensity, Light.InnerCone, Light.OuterCone});
    }

    for (size_t i = 0; i < Mesh.MaterialData.size(); ++i)
    {
        Engine::Model::MaterialData &Data = Mesh.MaterialData[i];
        Engine::Material *NewMaterial = new Engine::Material("Assets/Shaders/Deferred/Vert.glsl", "Assets/Shaders/Deferred/Frag.glsl");
        NewMaterial->SetUniform("Color", Data.DiffuseColor);
        if (!Data.DiffuseTextures.empty())
        {
            NewMaterial->LoadTexture(0, "Assets/Models/" + Data.DiffuseTextures[0]);
            NewMaterial->SetUniform("UseTexture", true);
        }
        if (!Data.NormalTextures.empty())
        {
            NewMaterial->LoadTexture(1, "Assets/Models/" + Data.NormalTextures[0]);
            NewMaterial->SetUniform("UseNormal", true);
        }
        if (!Data.SpecularTextures.empty())
        {
            NewMaterial->LoadTexture(2, "Assets/Models/" + Data.SpecularTextures[0]);
            NewMaterial->LoadTexture(3, "Assets/Models/" + Data.SpecularTextures[0]);
            NewMaterial->SetUniform("UseMetallic", true);
            NewMaterial->SetUniform("UseRoughness", true);
        }
        AssignedMaterials[i] = NewMaterial;
    }

    std::vector<Engine::Material *> MeshMaterials;
    for (const auto &MeshInstance : Mesh.Meshes)
        MeshMaterials.push_back(AssignedMaterials[MeshInstance.MaterialIndex]);

    Model = new Engine::Model::ModelInstance(Mesh, MeshMaterials, glm::mat4(1.0f));
}

void RenderModel()
{
    glm::mat4 ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -50.0f, 0.0f));
    ModelMatrix = glm::scale(ModelMatrix, glm::vec3(0.25f));
    Model->Transform = ModelMatrix;

    Engine::Model::DrawModelInstances({*Model}, &MainCamera);
}

void CalculateFPS()
{
    DeltaTime = glfwGetTime() - LastTime;
    LastTime = glfwGetTime();
    FPS = (DeltaTime > 0) ? 1.0f / DeltaTime : FPS;
}

void SetLights(const std::vector<Light>& Lights, const std::string& Prefix)
{
    RenderTargetSprite->GetMaterial()->SetUniform("Num" + Prefix, (int)Lights.size());

    for (unsigned int i = 0; i < Lights.size(); i++)
    {
        std::string Index = std::to_string(i);
        RenderTargetSprite->GetMaterial()->SetUniform((Prefix + "[" + Index + "].Color").c_str(), Lights[i].Color);
        RenderTargetSprite->GetMaterial()->SetUniform((Prefix + "[" + Index + "].Intensity").c_str(), Lights[i].Intensity);
        
        if (Prefix == "SpotLights")
        {
            RenderTargetSprite->GetMaterial()->SetUniform((Prefix + "[" + Index + "].Cutoff").c_str(), Lights[i].CutOff);
            RenderTargetSprite->GetMaterial()->SetUniform((Prefix + "[" + Index + "].OuterCutoff").c_str(), Lights[i].OuterCutOff);
        }
        
        if (Prefix == "SpotLights" || Prefix == "PointLights")
        {
            RenderTargetSprite->GetMaterial()->SetUniform((Prefix + "[" + Index + "].Position").c_str(), Lights[i].Position);
        }
        
        if (Prefix == "DirectionalLights")
        {
            RenderTargetSprite->GetMaterial()->SetUniform((Prefix + "[" + Index + "].Direction").c_str(), Lights[i].Direction);
        }
    }
}


void Render(GLFWwindow *Window)
{
    MainCamera.SetRotation(glm::rotate(glm::mat4(1.0f), (glm::float32)glm::radians(glfwGetTime() * 10.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
    glfwPollEvents();

    SceneRenderTarget->Resize(glm::vec2(WindowWidth, WindowHeight));
    SceneRenderTarget->Bind();

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    RenderModel();
    SceneRenderTarget->Unbind();

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Set the textures to the material for rendering the final image
    RenderTargetSprite->GetMaterial()->SetTexture(0, SceneRenderTarget->Textures[0]);
    RenderTargetSprite->GetMaterial()->SetTexture(1, SceneRenderTarget->Textures[1]);
    RenderTargetSprite->GetMaterial()->SetTexture(2, SceneRenderTarget->Textures[2]);
    RenderTargetSprite->GetMaterial()->SetTexture(3, SceneRenderTarget->Textures[4]);
    RenderTargetSprite->GetMaterial()->SetTexture(4, SceneRenderTarget->Textures[5]);
    RenderTargetSprite->GetMaterial()->SetTexture(5, SceneRenderTarget->Textures[6]);

    // Pass texture uniforms to the shader
    RenderTargetSprite->GetMaterial()->SetUniform("NormalTexture", 1);
    RenderTargetSprite->GetMaterial()->SetUniform("PositionTexture", 2);
    RenderTargetSprite->GetMaterial()->SetUniform("MetallicTexture", 3);
    RenderTargetSprite->GetMaterial()->SetUniform("RoughnessTexture", 4);
    RenderTargetSprite->GetMaterial()->SetUniform("EmissionTexture", 5);
    RenderTargetSprite->GetMaterial()->SetUniform("ViewPosition", MainCamera.GetPosition());

    // Call SetLights for each light type
    SetLights(PointLights, "PointLights");
    SetLights(DirectionalLights, "DirectionalLights");
    SetLights(Spotlights, "SpotLights");

    // Render the sprite with the updated material (which has all the light uniforms)
    RenderTargetSprite->SetSize(glm::vec2(WindowWidth, WindowHeight));
    RenderTargetSprite->Render();

    // FPS Calculation and Display
    CalculateFPS();
    std::stringstream SS;
    SS << "FPS: " << static_cast<int>(FPS);
    RenderText(SS.str());

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
        Render(Window);

    for (auto &mat : Model->Materials)
        delete mat;
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
    