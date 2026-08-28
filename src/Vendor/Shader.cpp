#include "Shader.h"
#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
#include "Window.h"
#include <glm/gtc/type_ptr.hpp>

ShaderProgramSource Shader::ParseShader(const std::string& filepath) {
    enum class ShaderType { // enum to determine the index in the string stream array
        NONE = -1, VERTEX = 0, FRAGMENT = 1
    };

    std::fstream stream;
    stream.open(filepath);
    if (!stream.is_open()) {
        std::cout << "Error: File is not opened: " << filepath << std::endl;
        return{ "","" };
    }

    std::string line;
    std::stringstream ss[2];
    ShaderType type = ShaderType::NONE;

    while (getline(stream, line))
    {
        if (line.find("#shader") != std::string::npos) {
            if (line.find("vertex") != std::string::npos) {
                //set mode to vertex
                type = ShaderType::VERTEX;

            }
            else if (line.find("fragment") != std::string::npos) {
                // set mode to fragment
                type = ShaderType::FRAGMENT;
            }
        }
        else {
            ss[static_cast<int>(type)] << line << '\n';
        }
    }
    stream.close();
    return { ss[0].str(),ss[1].str() };
}

std::string Shader::LoadShaderFromFile(const std::string& filepath) {
    std::ifstream stream(filepath);
    if (!stream.is_open()) {
        std::cout << "Error: Cannot open shader file: " << filepath << std::endl;
        return "";
    }

    std::stringstream buffer;
    buffer << stream.rdbuf();
    stream.close();

    return buffer.str();
}

unsigned int Shader::CompileShader(unsigned int type, const std::string& source) {
    unsigned int id = glCreateShader(type);
    const char* src = &source[0];
    GLCall(glShaderSource(id, 1, &src, nullptr));
    GLCall(glCompileShader(id));

    // Error handling
    int result;
    GLCall(glGetShaderiv(id, GL_COMPILE_STATUS, &result));
    if (result == GL_FALSE) {
        int length;
        GLCall(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length));
        char* message = (char*)alloca(length * sizeof(char)); // allocate on stack dynamically

        GLCall(glGetShaderInfoLog(id, length, &length, message));

        std::string shaderType;
        switch (type) {
        case GL_VERTEX_SHADER: shaderType = "Vertex"; break;
        case GL_FRAGMENT_SHADER: shaderType = "Fragment"; break;
        case GL_TESS_CONTROL_SHADER: shaderType = "Tessellation Control"; break;
        case GL_TESS_EVALUATION_SHADER: shaderType = "Tessellation Evaluation"; break;
        default: shaderType = "Unknown"; break;
        }

        std::cout << "Failed to compile " << shaderType << " shader: " << message << std::endl;
        GLCall(glDeleteShader(id));
        return 0;
    }
    return id;
}

unsigned int Shader::CreateShader(const std::string& vertexShader, const std::string& fragmentShader) {
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    if (vs == 0 || fs == 0) {
        std::cout << "Failed to compile shader program!" << std::endl;
        if (vs == 0) std::cout << "  - Vertex shader compilation failed" << std::endl;
        if (fs == 0) std::cout << "  - Fragment shader compilation failed" << std::endl;
        glDeleteProgram(program);
        return 0;
    }

    GLCall(glAttachShader(program, vs));
    GLCall(glAttachShader(program, fs));

    GLCall(glLinkProgram(program));
    int linkStatus;
    GLCall(glGetProgramiv(program, GL_LINK_STATUS, &linkStatus));

    if (linkStatus == GL_FALSE) {
        int length;
        GLCall(glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length));
        char* message = (char*)alloca(length * sizeof(char));
        GLCall(glGetProgramInfoLog(program, length, &length, message));
        std::cerr << "Failed to link shader program: " << message << std::endl;
    }

    glValidateProgram(program);

    GLCall(glDeleteShader(vs));
    GLCall(glDeleteShader(fs));

    return program;
}

int Shader::GetUniformLocation(const std::string name) {
    if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
        return m_UniformLocationCache[name];
    int location = glGetUniformLocation(m_RendererID, name.c_str());
    if (location == -1) {
        std::cout << "Warning: Uniform " << name << " doesn't exist." << std::endl;
    }
    m_UniformLocationCache[name] = location;
    return location;
}

Shader::~Shader() {
    if (m_RendererID) GLCall(glDeleteProgram(m_RendererID));
}

Shader::Shader(Shader&& other) noexcept
    : m_RendererID(other.m_RendererID), m_UniformLocationCache(std::move(other.m_UniformLocationCache)) {
    other.m_RendererID = 0;
}

Shader& Shader::operator=(Shader&& other) noexcept {
    if (this != &other) {
        if (m_RendererID) glDeleteProgram(m_RendererID);
        m_RendererID = other.m_RendererID;
        m_UniformLocationCache = std::move(other.m_UniformLocationCache);
        other.m_RendererID = 0;
    }
    return *this;
}

void Shader::Bind() const {
    GLCall(glUseProgram(m_RendererID));
}

void Shader::Unbind() const {
    GLCall(glUseProgram(0));
}

void Shader::SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3) {
    GLCall(glUniform4f(GetUniformLocation(name), v0, v1, v2, v3));
}

void Shader::SetUniform3f(const std::string& name, float v0, float v1, float v2) {
    GLCall(glUniform3f(GetUniformLocation(name), v0, v1, v2));
}

void Shader::SetUniform1f(const std::string& name, float value) {
    GLCall(glUniform1f(GetUniformLocation(name), value));
}

void Shader::SetUniform1i(const std::string& name, int value) {
    GLCall(glUniform1i(GetUniformLocation(name), value));
}

void Shader::setUniformMat4f(const std::string& name, glm::mat4& matrix) {
    GLCall(glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &matrix[0][0]));
}

void Shader::SetUniformMat4f(const std::string& name, const glm::mat4& matrix) {
    GLCall(glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &matrix[0][0]));
}

void Shader::setUniformMatrix4fv(const std::string& name, int count, bool transpose, glm::mat4 matrix) {
    glUniformMatrix4fv(GetUniformLocation(name), count, transpose, glm::value_ptr(matrix));
}

void Shader::SetUniform3v(const std::string& name, const glm::vec3& vector){
    GLCall(glUniform3fv(GetUniformLocation(name), 1, &vector[0]));
}

