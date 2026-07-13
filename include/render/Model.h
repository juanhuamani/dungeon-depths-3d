#pragma once
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <glad/gl.h>

namespace render {

struct Vertex {
    glm::vec3 Position;
    glm::vec2 TexCoords;
    glm::vec3 Normal;
};

class Model {
public:
    Model();
    ~Model();

    // Loads simple OBJ files
    bool loadOBJ(const std::string& path);
    void draw() const;

private:
    void setupMesh();

    std::vector<Vertex> m_Vertices;
    std::vector<unsigned int> m_Indices;
    
    unsigned int m_VAO, m_VBO, m_EBO;
    bool m_IsInitialized;
};

} // namespace render
