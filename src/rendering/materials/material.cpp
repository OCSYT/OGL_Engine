#include "Material.h"

Engine::Material::Material(const std::string& VertexPath, const std::string& FragmentPath, const std::vector<std::string>& TexturePaths) : Shader(VertexPath, FragmentPath) {
    LoadTextures(TexturePaths);
    SetUniform("Color", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
}

Engine::Material::~Material() {
    UnloadTextures();
}

Engine::Shader Engine::Material::GetShader(){
    return Shader;
}

void Engine::Material::Bind() const {
    Shader.Bind();
    
    for (size_t i = 0; i < TextureIDs.size(); ++i) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, TextureIDs[i]);
    }
}

void Engine::Material::SetUniform(const std::string &Name, int Value) {
    Shader.Bind();
    Shader.SetUniform(Name, Value);
}

void Engine::Material::SetUniform(const std::string &Name, float Value) {
    Shader.Bind();
    Shader.SetUniform(Name, Value);
}

void Engine::Material::SetUniform(const std::string &Name, const glm::vec3 &Value) {
    Shader.Bind();
    Shader.SetUniform(Name, Value);
}

void Engine::Material::SetUniform(const std::string &Name, const glm::vec4 &Value) {
    Shader.Bind();
    Shader.SetUniform(Name, Value);
}

void Engine::Material::SetUniform(const std::string &Name, const glm::mat4 &Value) {
    Shader.Bind();
    Shader.SetUniform(Name, Value);
}

void Engine::Material::SetTexture(int Unit, const std::string& TexturePath) {
    if (Unit < 0 || Unit >= static_cast<int>(TextureIDs.size())) {
        std::cerr << "Texture unit out of range!" << std::endl;
        return;
    }

    // Load new texture into the texture unit
    Util::UnloadTexture(TextureIDs[Unit]);
    unsigned int NewTextureID = Util::LoadTexture(TexturePath.c_str());
    if (NewTextureID) {
        glActiveTexture(GL_TEXTURE0 + Unit);
        glBindTexture(GL_TEXTURE_2D, NewTextureID);
        TextureIDs[Unit] = NewTextureID;
    }
}

void Engine::Material::SetShader(const std::string& VertexPath, const std::string& FragmentPath) {
    Shader.Unload();  
    Shader = Engine::Shader(VertexPath, FragmentPath);
}


void Engine::Material::LoadTextures(const std::vector<std::string>& TexturePaths) {
    UnloadTextures();

    for (const std::string& TexturePath : TexturePaths) {

        unsigned int TextureID = Util::LoadTexture(TexturePath);
        if (TextureID) {
            TextureIDs.push_back(TextureID);
        }
    }
}


void Engine::Material::UnloadTextures() {
    for (unsigned int TextureID : TextureIDs) {
        Util::UnloadTexture(TextureID);
    }
    TextureIDs.clear();
}

void Engine::Material::RemoveTexture(int Index) {
    if (Index < 0 || Index >= static_cast<int>(TextureIDs.size())) {
        std::cerr << "Invalid texture index!" << std::endl;
        return;
    }

    Util::UnloadTexture(TextureIDs[Index]);
    TextureIDs.erase(TextureIDs.begin() + Index);
}


void Engine::Material::ReloadTexture(int Index, const std::string& NewTexturePath) {
    if (Index < 0 || Index >= static_cast<int>(TextureIDs.size())) {
        std::cerr << "Invalid texture index!" << std::endl;
        return;
    }

    unsigned int NewTextureID = Util::LoadTexture(NewTexturePath.c_str());
    if (NewTextureID) {
        Util::UnloadTexture(TextureIDs[Index]);
        TextureIDs[Index] = NewTextureID;
    }
}
