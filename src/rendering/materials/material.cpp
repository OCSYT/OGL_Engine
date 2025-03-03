#include "Material.h"

Material::Material(const std::string& VertexPath, const std::string& FragmentPath, const std::vector<std::string>& TexturePaths)
    : Shader(VertexPath, FragmentPath) {
    
    LoadTextures(TexturePaths);
}

Material::~Material() {
    UnloadTextures();
}

void Material::Bind() const {
    Shader.Bind();
    
    for (size_t i = 0; i < TextureIDs.size(); ++i) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, TextureIDs[i]);
    }
}

void Material::SetUniform(const std::string &Name, int Value) {
    Shader.Bind();
    Shader.SetUniform(Name, Value);
}

void Material::SetUniform(const std::string &Name, float Value) {
    Shader.Bind();
    Shader.SetUniform(Name, Value);
}

void Material::SetUniform(const std::string &Name, const glm::vec3 &Value) {
    Shader.Bind();
    Shader.SetUniform(Name, Value);
}

void Material::SetUniform(const std::string &Name, const glm::vec4 &Value) {
    Shader.Bind();
    Shader.SetUniform(Name, Value);
}

void Material::SetUniform(const std::string &Name, const glm::mat4 &Value) {
    Shader.Bind();
    Shader.SetUniform(Name, Value);
}

void Material::LoadTextures(const std::vector<std::string>& TexturePaths) {
    UnloadTextures();  // Unload existing textures before loading new ones

    for (const auto& TexturePath : TexturePaths) {
        std::filesystem::path FullPath = std::filesystem::path(Util::GetExecutablePath()) / TexturePath;
        std::string FullPathStr = FullPath.string();
        const char* Path = FullPathStr.c_str();
        
        unsigned int TextureID = Util::LoadTexture(Path);
        if (TextureID) {
            TextureIDs.push_back(TextureID);
        }
    }
}

void Material::UnloadTextures() {
    if (!TextureIDs.empty()) {
        glDeleteTextures(static_cast<GLsizei>(TextureIDs.size()), TextureIDs.data());
        TextureIDs.clear();
    }
}
