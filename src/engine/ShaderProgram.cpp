#include "engine/ShaderProgram.h"

#include <glm/gtc/type_ptr.hpp>

#include <fstream>
#include <sstream>
#include <iostream>

ShaderProgram::~ShaderProgram()
{
    if (m_programID != 0)
        glDeleteProgram(m_programID);
}

ShaderProgram::ShaderProgram(ShaderProgram&& other) noexcept
    : m_programID(other.m_programID)
{
    other.m_programID = 0;
}

ShaderProgram& ShaderProgram::operator=(ShaderProgram&& other) noexcept
{
    if (this != &other)
    {
        if (m_programID != 0)
            glDeleteProgram(m_programID);
        m_programID = other.m_programID;
        other.m_programID = 0;
    }
    return *this;
}

bool ShaderProgram::loadFromFiles(const std::string& vertexPath, const std::string& fragmentPath)
{
    std::string vertSrc = readFile(vertexPath);
    std::string fragSrc = readFile(fragmentPath);

    if (vertSrc.empty() || fragSrc.empty())
    {
        std::cerr << "ShaderProgram: Error leyendo archivos de shader\n";
        return false;
    }

    GLuint vert = compileShader(GL_VERTEX_SHADER, vertSrc);
    GLuint frag = compileShader(GL_FRAGMENT_SHADER, fragSrc);

    if (vert == 0 || frag == 0)
    {
        if (vert) glDeleteShader(vert);
        if (frag) glDeleteShader(frag);
        return false;
    }

    m_programID = glCreateProgram();
    glAttachShader(m_programID, vert);
    glAttachShader(m_programID, frag);
    glLinkProgram(m_programID);

    int success;
    glGetProgramiv(m_programID, GL_LINK_STATUS, &success);
    if (!success)
    {
        char log[512];
        glGetProgramInfoLog(m_programID, 512, nullptr, log);
        std::cerr << "ShaderProgram: Error de linkeo:\n" << log << "\n";
        glDeleteProgram(m_programID);
        m_programID = 0;
    }

    glDeleteShader(vert);
    glDeleteShader(frag);

    return m_programID != 0;
}

void ShaderProgram::use() const
{
    glUseProgram(m_programID);
}

void ShaderProgram::setInt(const std::string& name, int value) const
{
    glUniform1i(glGetUniformLocation(m_programID, name.c_str()), value);
}

void ShaderProgram::setFloat(const std::string& name, float value) const
{
    glUniform1f(glGetUniformLocation(m_programID, name.c_str()), value);
}

void ShaderProgram::setVec3(const std::string& name, const glm::vec3& value) const
{
    glUniform3fv(glGetUniformLocation(m_programID, name.c_str()), 1, glm::value_ptr(value));
}

void ShaderProgram::setVec4(const std::string& name, const glm::vec4& value) const
{
    glUniform4fv(glGetUniformLocation(m_programID, name.c_str()), 1, glm::value_ptr(value));
}

void ShaderProgram::setMat4(const std::string& name, const glm::mat4& value) const
{
    glUniformMatrix4fv(glGetUniformLocation(m_programID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}

std::string ShaderProgram::readFile(const std::string& path)
{
    std::ifstream file(path);
    if (!file.is_open())
    {
        std::cerr << "ShaderProgram: No se pudo abrir '" << path << "'\n";
        return "";
    }
    std::stringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

GLuint ShaderProgram::compileShader(GLenum type, const std::string& source)
{
    GLuint shader = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        char log[512];
        glGetShaderInfoLog(shader, 512, nullptr, log);
        const char* typeName = (type == GL_VERTEX_SHADER) ? "VERTEX" : "FRAGMENT";
        std::cerr << "ShaderProgram: Error compilando " << typeName << ":\n" << log << "\n";
        glDeleteShader(shader);
        return 0;
    }

    return shader;
}
