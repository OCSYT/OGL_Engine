#include "util.h"

std::string Engine::Util::GetExecutablePath() {
    char path[MAX_PATH];
    GetModuleFileNameA(nullptr, path, MAX_PATH);
    return std::filesystem::path(path).parent_path().string();
}


unsigned int Engine::Util::LoadTexture(std::string Path) {
    std::filesystem::path FullPath = std::filesystem::path(Util::GetExecutablePath()) / Path;
    std::string FullPathStr = FullPath.string();
    const char* FullPathString = FullPathStr.c_str();
    stbi_set_flip_vertically_on_load(true);  
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
    unsigned char *Data = stbi_load(FullPathString, &Width, &Height, &NumChannels, 0);
    if (Data) {
        GLenum Format = (NumChannels == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, Format, Width, Height, 0, Format, GL_UNSIGNED_BYTE, Data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cerr << "Failed to load texture: " << Path << std::endl;
    }

    stbi_image_free(Data);
    return TextureID;
}

unsigned int Engine::Util::LoadTextureFromData(const unsigned char* Data, int Width, int Height, int NumChannels) {
    unsigned int TextureID;
    glGenTextures(1, &TextureID);
    glBindTexture(GL_TEXTURE_2D, TextureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    GLenum Format = (NumChannels == 4) ? GL_RGBA : GL_RGB;

    if (NumChannels != 3 && NumChannels != 4 || Width <= 0 || Height <= 0 || Data == nullptr) {
        return 0; // Invalid parameters, return 0
    }

    glTexImage2D(GL_TEXTURE_2D, 0, Format, Width, Height, 0, Format, GL_UNSIGNED_BYTE, Data);
    glGenerateMipmap(GL_TEXTURE_2D);

    return TextureID;
}


void Engine::Util::UnloadTexture(unsigned int& TextureID) {
    glDeleteTextures(1, &TextureID);
    TextureID = 0;
}