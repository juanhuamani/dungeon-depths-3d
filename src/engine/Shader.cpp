#include "engine/Shader.h"

#include <fstream>
#include <iostream>
#include <sstream>

namespace engine {

namespace {

std::string readFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Shader: no se pudo abrir " << path << '\n';
        return {};
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

} // namespace

Shader::~Shader() {
    if (programId_ != 0) {
        glDeleteProgram(programId_);
        programId_ = 0;
    }
}

bool Shader::loadFromFiles(const std::string& vertexPath, const std::string& fragmentPath) {
    const std::string vertexSource = readFile(vertexPath);
    const std::string fragmentSource = readFile(fragmentPath);
    if (vertexSource.empty() || fragmentSource.empty()) {
        return false;
    }

    GLuint vertexShader = 0;
    GLuint fragmentShader = 0;
    if (!compileStage(GL_VERTEX_SHADER, vertexSource, vertexShader)) {
        return false;
    }
    if (!compileStage(GL_FRAGMENT_SHADER, fragmentSource, fragmentShader)) {
        glDeleteShader(vertexShader);
        return false;
    }

    const bool linked = linkProgram(vertexShader, fragmentShader);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return linked;
}

bool Shader::compileStage(GLenum type, const std::string& source, GLuint& outShader) const {
    outShader = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(outShader, 1, &src, nullptr);
    glCompileShader(outShader);

    GLint success = 0;
    glGetShaderiv(outShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char log[512];
        glGetShaderInfoLog(outShader, sizeof(log), nullptr, log);
        std::cerr << "Shader compile error:\n" << log << '\n';
        glDeleteShader(outShader);
        outShader = 0;
        return false;
    }
    return true;
}

bool Shader::linkProgram(GLuint vertexShader, GLuint fragmentShader) {
    if (programId_ != 0) {
        glDeleteProgram(programId_);
        programId_ = 0;
    }

    programId_ = glCreateProgram();
    glAttachShader(programId_, vertexShader);
    glAttachShader(programId_, fragmentShader);
    glLinkProgram(programId_);

    GLint success = 0;
    glGetProgramiv(programId_, GL_LINK_STATUS, &success);
    if (!success) {
        char log[512];
        glGetProgramInfoLog(programId_, sizeof(log), nullptr, log);
        std::cerr << "Shader link error:\n" << log << '\n';
        glDeleteProgram(programId_);
        programId_ = 0;
        return false;
    }
    return true;
}

void Shader::use() const {
    glUseProgram(programId_);
}

void Shader::setMat4(const std::string& name, const glm::mat4& value) const {
    const GLint location = glGetUniformLocation(programId_, name.c_str());
    if (location >= 0) {
        glUniformMatrix4fv(location, 1, GL_FALSE, &value[0][0]);
    }
}

} // namespace engine
