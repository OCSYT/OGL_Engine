#define STB_IMAGE_IMPLEMENTATION
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "util/util.h"
#include <filesystem>
#include "rendering/camera/camera.h"
#include "rendering/materials/material.h"

const unsigned int WindowWidth = 800;
const unsigned int WindowHeight = 600;
Camera MainCamera(CameraMode::Perspective, (float)WindowWidth / (float)WindowHeight);

const float Vertices[] = {
    // positions          // colors           // texture coords
    0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 1.0f,   1.0f, 1.0f,   // top right
    0.5f, -0.5f, 0.0f,   1.0f, 1.0f, 1.0f,   1.0f, 0.0f,   // bottom right
    -0.5f, -0.5f, 0.0f,  1.0f, 1.0f, 1.0f,   0.0f, 0.0f,   // bottom left
    -0.5f,  0.5f, 0.0f,  1.0f, 1.0f, 1.0f,   0.0f, 1.0f    // top left 
};
unsigned int Indices[] = {  
    0, 1, 3, // first triangle
    1, 2, 3  // second triangle
};

void FramebufferSizeCallback(GLFWwindow *Window, int Width, int Height) {
    float WindowAspectRatio = (float)WindowWidth / (float)WindowHeight;
    float CurrentAspectRatio = (float)Width / (float)Height;
    int NewWidth, NewHeight, XOffset, YOffset;
    if (CurrentAspectRatio > WindowAspectRatio) {
        NewWidth = (int)(Height * WindowAspectRatio);
        NewHeight = Height;
        XOffset = (Width - NewWidth) / 2;
        YOffset = 0;
    } else {
        NewWidth = Width;
        NewHeight = (int)(Width / WindowAspectRatio);
        XOffset = 0;
        YOffset = (Height - NewHeight) / 2;
    }
    glViewport(XOffset, YOffset, NewWidth, NewHeight);
}

int main() {
    MainCamera.SetPosition(glm::vec3(0.0f, 0.0f, 3.0f));
    MainCamera.SetPerspective(60.0f, 0.1f, 100.0f);

    if (!glfwInit()) {
        std::cerr << "Could not initialize GLFW" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *Window = glfwCreateWindow(WindowWidth, WindowHeight, "Unlit Rendering", nullptr, nullptr);
    if (!Window) {
        std::cerr << "Could not create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(Window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Could not initialize GLAD" << std::endl;
        return -1;
    }

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

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    Material material("shaders/main/vert.glsl", "shaders/main/frag.glsl", { "silly.png" });

    
    glClearColor(0, 0, 0, 1.0f);

    while (!glfwWindowShouldClose(Window)) {
        glfwPollEvents();
        glClear(GL_COLOR_BUFFER_BIT);
        material.Bind();

        glm::mat4 View = MainCamera.GetViewMatrix();
        glm::mat4 Projection = MainCamera.GetProjectionMatrix();
        glm::mat4 Model = glm::mat4(1.0f);
        material.SetUniform("View", View);
        material.SetUniform("Projection", Projection);
        material.SetUniform("Model", Model);
        material.SetUniform("UseTexture", true);

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
