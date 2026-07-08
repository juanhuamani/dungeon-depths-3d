#pragma once

#include <glad/gl.h>
#include <glm/glm.hpp>

#include <string>

class ShaderProgram {
public:
    ShaderProgram() = default;
    ~ShaderProgram();
    ShaderProgram(const ShaderProgram&) = delete;
    ShaderProgram& operator=(const ShaderProgram&) = delete;
    ShaderProgram(ShaderProgram&& other) noexcept;
    ShaderProgram& operator=(ShaderProgram&& other) noexcept;
    bool loadFromFiles(const std::string& vertexPath, const std::string& fragmentPath);
    void use() const;
    GLuint getID() const { return m_programID; }

    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
    void setVec3(const std::string& name, const glm::vec3& value) const;
    void setVec4(const std::string& name, const glm::vec4& value) const;
    void setMat4(const std::string& name, const glm::mat4& value) const;

private:
    GLuint m_programID = 0;
    static std::string readFile(const std::string& path);
    static GLuint compileShader(GLenum type, const std::string& source);
};
