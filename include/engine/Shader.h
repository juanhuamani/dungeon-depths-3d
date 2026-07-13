#pragma once

#include <glad/gl.h>
#include <glm/glm.hpp>

#include <string>

namespace engine {

class Shader {
public:
    Shader() = default;
    ~Shader();

    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    bool loadFromFiles(const std::string& vertexPath, const std::string& fragmentPath);
    void use() const;
    void setMat4(const std::string& name, const glm::mat4& value) const;
    void setVec3(const std::string& name, const glm::vec3& value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
    void destroy();

    GLuint id() const { return programId_; }

private:
    bool compileStage(GLenum type, const std::string& source, GLuint& outShader) const;
    bool linkProgram(GLuint vertexShader, GLuint fragmentShader);

    GLuint programId_ = 0;
};

} // namespace engine
