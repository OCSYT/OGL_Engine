#include "Material.h"

Material::Material(const std::string& VertexPath, const std::string& FragmentPath, const std::vector<std::string>& TexturePaths)
    : Shader(VertexPath, FragmentPath) {
    
    LoadTextures(TexturePaths);
}

Material::~Material() {
    UnloadTextures();
}

Shader Material::GetShader(){
    return Shader;
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

void Material::SetTexture(int Unit, const std::string& TexturePath) {
    if (Unit < 0 || Unit >= static_cast<int>(TextureIDs.size())) {
        std::cerr << "Texture unit out of range!" << std::endl;
        return;
    }

    // Load new texture into the texture unit
    unsigned int NewTextureID = Util::LoadTexture(TexturePath.c_str());
    if (NewTextureID) {
        glActiveTexture(GL_TEXTURE0 + Unit);
        glBindTexture(GL_TEXTURE_2D, NewTextureID);
        TextureIDs[Unit] = NewTextureID;
    }
}

void Material::SetShader(const std::string& VertexPath, const std::string& FragmentPath) {
    return;
    Shader.Unload();  
    Shader = ::Shader(VertexPath, FragmentPath);
}


void Material::LoadTextures(const std::vector<std::string>& TexturePaths) {
    UnloadTextures();

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

void Material::RemoveTexture(int Index) {
    if (Index < 0 || Index >= static_cast<int>(TextureIDs.size())) {
        std::cerr << "Invalid texture index!" << std::endl;
        return;
    }

    glDeleteTextures(1, &TextureIDs[Index]);
    TextureIDs.erase(TextureIDs.begin() + Index);
}

void Material::ReloadTexture(int Index, const std::string& NewTexturePath) {
    if (Index < 0 || Index >= static_cast<int>(TextureIDs.size())) {
        std::cerr << "Invalid texture index!" << std::endl;
        return;
    }

    unsigned int NewTextureID = Util::LoadTexture(NewTexturePath.c_str());
    if (NewTextureID) {
        glDeleteTextures(1, &TextureIDs[Index]);
        TextureIDs[Index] = NewTextureID;
    }
}
