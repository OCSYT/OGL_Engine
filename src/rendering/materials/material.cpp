#include "Material.h"

Engine::Material::Material(
    const std::string& VertexPath,
    const std::string& FragmentPath,
    const std::vector<std::string>& TexturePaths,
    const std::vector<std::pair<GLint, GLint>>& FilterOptions)
    : Shader(VertexPath, FragmentPath)
{
    std::vector<std::pair<GLint, GLint>> ValidFilterOptions = FilterOptions;
    if (ValidFilterOptions.empty()) {
        ValidFilterOptions.resize(TexturePaths.size(), { GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR });
    } else if (ValidFilterOptions.size() < TexturePaths.size()) {
        ValidFilterOptions.resize(TexturePaths.size(), { GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR });
    }

    LoadTextures(TexturePaths, ValidFilterOptions);
    SetUniform("Color", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
}



Engine::Material::~Material() {
    UnloadTextures();
}

Engine::Shader* Engine::Material::GetShader(){
    return &Shader;
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

void Engine::Material::LoadTexture(int Unit, const std::string& TexturePath, GLint MinFilter, GLint MagFilter) {
    if (Unit < 0 || Unit >= static_cast<int>(TextureIDs.size())) {        
        if (Unit >= static_cast<int>(TextureIDs.size())) {
            TextureIDs.resize(Unit + 1, 0);
        }
    }

    // Load new texture into the texture unit with filtering options
    Util::UnloadTexture(TextureIDs[Unit]);
    unsigned int NewTextureID = Util::LoadTexture(TexturePath.c_str(), MinFilter, MagFilter);
    if (NewTextureID) {
        glActiveTexture(GL_TEXTURE0 + Unit);
        glBindTexture(GL_TEXTURE_2D, NewTextureID);
        TextureIDs[Unit] = NewTextureID;
    }
}



void Engine::Material::SetTexture(int Unit, unsigned int TextureID) {
    if (Unit < 0 || Unit >= static_cast<int>(TextureIDs.size())) {        
        if (Unit >= static_cast<int>(TextureIDs.size())) {
            TextureIDs.resize(Unit + 1, 0);
        }
    }


    glActiveTexture(GL_TEXTURE0 + Unit);
    glBindTexture(GL_TEXTURE_2D, TextureID);

    TextureIDs[Unit] = TextureID;
}

void Engine::Material::SetShader(const std::string& VertexPath, const std::string& FragmentPath) {
    Shader.Unload();  
    Shader = Engine::Shader(VertexPath, FragmentPath);
}


void Engine::Material::LoadTextures(const std::vector<std::string>& TexturePaths, const std::vector<std::pair<GLint, GLint>>& FilterOptions) {
    for (size_t i = 0; i < TexturePaths.size(); ++i) {
        GLint MinFilter = GL_LINEAR_MIPMAP_LINEAR;
        GLint MagFilter = GL_LINEAR;

        if (i < FilterOptions.size()) {
            MinFilter = FilterOptions[i].first;
            MagFilter = FilterOptions[i].second;
        }

        unsigned int TextureID = Util::LoadTexture(TexturePaths[i], MinFilter, MagFilter);
        if (TextureID) {
            TextureIDs.push_back(TextureID);
        }
    }
}




void Engine::Material::LoadTexturesFromData(
    const std::vector<std::tuple<const unsigned char*, int, int, int>>& TextureData,
    const std::vector<std::pair<GLint, GLint>>& FilterOptions) 
{
    for (size_t i = 0; i < TextureData.size(); ++i) {
        const auto& DataTuple = TextureData[i];
        const unsigned char* Data = std::get<0>(DataTuple);
        int Width = std::get<1>(DataTuple);
        int Height = std::get<2>(DataTuple);
        int NumChannels = std::get<3>(DataTuple);

        GLint MinFilter = GL_LINEAR_MIPMAP_LINEAR;
        GLint MagFilter = GL_LINEAR;
        if (i < FilterOptions.size()) {
            MinFilter = FilterOptions[i].first;
            MagFilter = FilterOptions[i].second;
        }

        unsigned int TextureID = Util::LoadTextureFromData(Data, Width, Height, NumChannels, MinFilter, MagFilter);
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

