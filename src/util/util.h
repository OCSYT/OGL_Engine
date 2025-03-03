#pragma once

#include <iostream>
#include <string>
#include <filesystem>
#include <windows.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

class Util {
public:
    static std::string GetExecutablePath();
    static unsigned int LoadTexture(const char *Path);
};
