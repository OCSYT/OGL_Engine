#include "util.h"

std::string Engine::Util::GetExecutablePath() {
    char path[MAX_PATH];
    GetModuleFileNameA(nullptr, path, MAX_PATH);
    return std::filesystem::path(path).parent_path().string();
}


unsigned int Engine::Util::LoadTexture(std::string Path, GLint MinFilter, GLint MagFilter) {
    std::filesystem::path FullPath = std::filesystem::path(Util::GetExecutablePath()) / Path;
    std::string FullPathStr = FullPath.string();
    const char* FullPathString = FullPathStr.c_str();
    stbi_set_flip_vertically_on_load(true);  
    unsigned int TextureID;
    glGenTextures(1, &TextureID);
    glBindTexture(GL_TEXTURE_2D, TextureID);

    // Set texture wrapping
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Apply user-defined filtering options
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, MinFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, MagFilter);

    // Load image data
    int Width, Height, NumChannels;
    unsigned char *Data = stbi_load(FullPathString, &Width, &Height, &NumChannels, 0);
    if (Data) {
        GLenum Format = (NumChannels == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, Format, Width, Height, 0, Format, GL_UNSIGNED_BYTE, Data);
        
        if (MinFilter == GL_LINEAR_MIPMAP_LINEAR || MinFilter == GL_NEAREST_MIPMAP_NEAREST || MinFilter == GL_NEAREST_MIPMAP_LINEAR || MinFilter == GL_LINEAR_MIPMAP_NEAREST) {
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        std::cout << "Loaded texture: " << Path << std::endl;
    } else {
        std::cerr << "Failed to load texture: " << Path << std::endl;
    }

    stbi_image_free(Data);
    return TextureID;
}

unsigned int Engine::Util::LoadTextureFromData(const unsigned char* Data, int Width, int Height, int NumChannels, GLint MinFilter, GLint MagFilter) {
    unsigned int TextureID;
    glGenTextures(1, &TextureID);
    glBindTexture(GL_TEXTURE_2D, TextureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, MinFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, MagFilter);

    GLenum Format = (NumChannels == 4) ? GL_RGBA : GL_RGB;

    if (NumChannels != 3 && NumChannels != 4 || Width <= 0 || Height <= 0 || Data == nullptr) {
        return 0; // Invalid parameters, return 0
    }

    glTexImage2D(GL_TEXTURE_2D, 0, Format, Width, Height, 0, Format, GL_UNSIGNED_BYTE, Data);
    
    if (MinFilter == GL_LINEAR_MIPMAP_LINEAR || MinFilter == GL_NEAREST_MIPMAP_NEAREST || MinFilter == GL_NEAREST_MIPMAP_LINEAR || MinFilter == GL_LINEAR_MIPMAP_NEAREST) {
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    return TextureID;
}



void Engine::Util::UnloadTexture(unsigned int& TextureID) {
    glDeleteTextures(1, &TextureID);
    TextureID = 0;
}