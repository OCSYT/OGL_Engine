#include "shader.h"

Engine::Shader::Shader(const std::string &vertexPath, const std::string &fragmentPath) {
    std::string vertexSource = LoadShaderSource(vertexPath);
    std::string fragmentSource = LoadShaderSource(fragmentPath);

    unsigned int vertexShaderId = CreateShader(GL_VERTEX_SHADER, vertexSource);
    unsigned int fragmentShaderId = CreateShader(GL_FRAGMENT_SHADER, fragmentSource);

    if (vertexShaderId != 0 && fragmentShaderId != 0) {
        ID = CreateProgram(vertexShaderId, fragmentShaderId);
        glDeleteShader(vertexShaderId);
        glDeleteShader(fragmentShaderId);
        if (ID == 0) { // Check if program creation failed
            std::cerr << "Failed to create shader program (linking error)" << std::endl;
        }
    } else {
        std::cerr << "Failed to create shader program (compilation error)" << std::endl;
        ID = 0;
    }
}

Engine::Shader::~Shader() {
    Unload();
}

void Engine::Shader::Bind() const {
    if (ID == 0) {
        std::cerr << "Error: Trying to bind an invalid shader program!" << std::endl;
        return;
    }
    glUseProgram(ID);
}


void Engine::Shader::Unbind() const {
    glUseProgram(0);
}

void Engine::Shader::Unload() {
    if (ID != 0) {
        glDeleteProgram(ID);
        ID = 0;
    }
}

unsigned int Engine::Shader::CreateShader(unsigned int shaderType, const std::string &shaderSource) {
    unsigned int shaderId = glCreateShader(shaderType);
    const char* source = shaderSource.c_str();
    glShaderSource(shaderId, 1, &source, nullptr);
    glCompileShader(shaderId);

    int status;
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &status);
    if (!status) {
        char infoLog[512];
        glGetShaderInfoLog(shaderId, sizeof(infoLog), nullptr, infoLog);
        std::cerr << "Shader compilation error:\n" << infoLog << std::endl;
        glDeleteShader(shaderId);
        return 0;
    }
    return shaderId;
}

unsigned int Engine::Shader::CreateProgram(unsigned int vertexShaderId, unsigned int fragmentShaderId) {
    if (vertexShaderId == 0 || fragmentShaderId == 0) {
        std::cerr << "Error: One or both shaders failed to compile!" << std::endl;
        return 0;
    }

    unsigned int programId = glCreateProgram();
    if (programId == 0) {
        std::cerr << "Error: glCreateProgram() failed!" << std::endl;
        return 0;
    }

    glAttachShader(programId, vertexShaderId);
    glAttachShader(programId, fragmentShaderId);
    glLinkProgram(programId);

    int status;
    glGetProgramiv(programId, GL_LINK_STATUS, &status);
    if (!status) {
        char infoLog[1024];
        glGetProgramInfoLog(programId, sizeof(infoLog), nullptr, infoLog);
        std::cerr << "Shader linking error:\n" << infoLog << std::endl;
        glDeleteProgram(programId);
        return 0;
    }

    std::cout << "Shader Program Linked Successfully. ID: " << programId << std::endl;
    return programId;
}

std::string Engine::Shader::LoadShaderSource(const std::string &filePath) {
    std::filesystem::path fullPath = std::filesystem::path(Util::GetExecutablePath()) / filePath;
    std::ifstream file(fullPath);
    
    if (!file) {
        std::cerr << "Failed to open shader file: " << fullPath << std::endl;
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source = buffer.str();
    if (source.empty()) {
        std::cerr << "Shader file is empty: " << fullPath << std::endl;
    }
    return source;
}


int Engine::Shader::GetUniformLocation(const std::string &name) {
    if (ID == 0) {
        std::cerr << "Error: Trying to get uniform '" << name << "' from an invalid shader program! (ID is 0)" << std::endl;
        return -1;
    }

    auto it = UniformCache.find(name);
    if (it != UniformCache.end()) {
        return it->second;
    }

    int location = glGetUniformLocation(ID, name.c_str());
    if (location == -1) {
        std::cerr << "Warning: Uniform '" << name << "' not found in shader program " << ID << "!" << std::endl;
    } else {
        UniformCache[name] = location;
    }

    return location;
}


void Engine::Shader::SetUniform(const std::string &name, int value) {
    glUniform1i(GetUniformLocation(name), value);
}

void Engine::Shader::SetUniform(const std::string &name, float value) {
    glUniform1f(GetUniformLocation(name), value);
}

void Engine::Shader::SetUniform(const std::string &name, const glm::vec2 &value) {
    glUniform2fv(GetUniformLocation(name), 1, &value[0]);
}


void Engine::Shader::SetUniform(const std::string &name, const glm::vec3 &value) {
    glUniform3fv(GetUniformLocation(name), 1, &value[0]);
}

void Engine::Shader::SetUniform(const std::string &name, const glm::vec4 &value) {
    glUniform4fv(GetUniformLocation(name), 1, &value[0]);
}

void Engine::Shader::SetUniform(const std::string &name, const glm::mat4 &value) {
    glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &value[0][0]);
}