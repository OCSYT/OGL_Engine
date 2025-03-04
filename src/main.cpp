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

unsigned int WindowWidth = 800;
unsigned int WindowHeight = 600;
unsigned int* PtrWindowWidth = &WindowWidth;
unsigned int* PtrWindowHeight = &WindowHeight;

Camera MainCamera(CameraMode::Perspective, PtrWindowWidth, PtrWindowHeight);

void FramebufferSizeCallback(GLFWwindow* Window, int Width, int Height) {
    if (Height == 0) return;
    WindowWidth = Width;
    WindowHeight = Height;
    glViewport(0, 0, WindowWidth, WindowHeight);
}

// Cube vertex data (Position, UV, Color)
float CubeVertices[] = {
    // Positions       // UV       // Colors
   -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  1.0f, 0.0f, 0.0f, // 0
    0.5f, -0.5f, -0.5f,  1.0f, 0.0f,  0.0f, 1.0f, 0.0f, // 1
    0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f, 0.0f, 1.0f, // 2
   -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  1.0f, 1.0f, 0.0f, // 3

   -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  1.0f, 0.0f, 1.0f, // 4
    0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.0f, 1.0f, 1.0f, // 5
    0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  1.0f, 1.0f, 1.0f, // 6
   -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,  0.0f, 0.0f, 0.0f  // 7
};

// Cube indices for drawing with EBO
unsigned int CubeIndices[] = {
    0, 1, 2,  2, 3, 0,  // Back face
    1, 5, 6,  6, 2, 1,  // Right face
    5, 4, 7,  7, 6, 5,  // Front face
    4, 0, 3,  3, 7, 4,  // Left face
    3, 2, 6,  6, 7, 3,  // Top face
    4, 5, 1,  1, 0, 4   // Bottom face
};

unsigned int CubeVAO, CubeVBO, CubeEBO;
Material* CubeMaterial;  // Using Material System

void InitCube() {
    glGenVertexArrays(1, &CubeVAO);
    glGenBuffers(1, &CubeVBO);
    glGenBuffers(1, &CubeEBO);

    glBindVertexArray(CubeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, CubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(CubeVertices), CubeVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, CubeEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(CubeIndices), CubeIndices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

    // Use material system
    CubeMaterial = new Material("assets/shaders/main/vert.glsl", "assets/shaders/main/frag.glsl", {});
}

void RenderCube() {
    CubeMaterial->Bind();

    glm::mat4 Model = glm::rotate(glm::mat4(1.0f), (float)glfwGetTime(), glm::vec3(0.5f, 1.0f, 0.0f));
    CubeMaterial->SetUniform("Model", Model);
    CubeMaterial->SetUniform("View", MainCamera.GetViewMatrix());
    CubeMaterial->SetUniform("Projection", MainCamera.GetProjectionMatrix());
    CubeMaterial->SetUniform("UseTexture", false);

    glBindVertexArray(CubeVAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}

int main() {
    if (!glfwInit()) return -1;
    GLFWwindow* Window = glfwCreateWindow(WindowWidth, WindowHeight, "3D Cube with Sprite", nullptr, nullptr);
    if (!Window) return -1;
    glfwMakeContextCurrent(Window);
    glfwSetFramebufferSizeCallback(Window, FramebufferSizeCallback);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) return -1;

    InitCube();
    Material material("assets/shaders/main/vert.glsl", "assets/shaders/main/frag.glsl", {"assets/sprites/sprite.png"});
    Sprite MySprite(material, glm::vec2(0, 0), glm::vec2(250.0f, 250.0f), PtrWindowWidth, PtrWindowHeight);
    MainCamera.SetPosition(glm::vec3(0, 0, 2));
    
    while (!glfwWindowShouldClose(Window)) {
        glfwPollEvents();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        RenderCube();

        MySprite.Render();
        MySprite.SetPosition(glm::vec2(WindowWidth - 275.0f, 25.0f));

        glfwSwapBuffers(Window);
    }

    delete CubeMaterial;
    glfwDestroyWindow(Window);
    glfwTerminate();
    return 0;
}
