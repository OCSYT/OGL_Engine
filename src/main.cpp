#define STB_IMAGE_IMPLEMENTATION
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "util/util.h"
#include <filesystem>
#include "Rendering/Camera/Camera.h"
#include "Rendering/Sprites/Sprite.h"
#include "Rendering/Materials/Material.h"

unsigned int WindowWidth = 800;
unsigned int WindowHeight = 600;
float AspectRatio = (float)WindowWidth / (float)WindowHeight;
Camera MainCamera(CameraMode::Perspective, AspectRatio);

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

    WindowWidth = NewWidth;
    WindowHeight = NewHeight;
    AspectRatio = CurrentAspectRatio;

    glViewport(XOffset, YOffset, WindowWidth, WindowHeight);
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

    GLFWwindow *Window = glfwCreateWindow(WindowWidth, WindowHeight, "Sprite Rendering", nullptr, nullptr);
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

    Material material("shaders/main/vert.glsl", "shaders/main/frag.glsl", {"silly.png"});
    Sprite MySprite(material, glm::vec2(-1.0f, -1.0f), glm::vec2(1.0f, 1.0f), Sprite::Anchor::TopLeft, WindowWidth, WindowHeight);
    glClearColor(0, 0, 0, 1.0f);

    while (!glfwWindowShouldClose(Window)) {
        glfwPollEvents();
        glClear(GL_COLOR_BUFFER_BIT);

        glm::mat4 View = MainCamera.GetViewMatrix();
        glm::mat4 Projection = MainCamera.GetProjectionMatrix();

        MySprite.Render(View, Projection);

        glfwSwapBuffers(Window);
    }

    glfwDestroyWindow(Window);
    glfwTerminate();
    return 0;
}
