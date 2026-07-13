#include "render/Model.h"
#include <iostream>
#include <fstream>
#include <sstream>

namespace render {

Model::Model() : m_VAO(0), m_VBO(0), m_EBO(0), m_IsInitialized(false) {}

Model::~Model() {
    if (m_IsInitialized) {
        glDeleteVertexArrays(1, &m_VAO);
        glDeleteBuffers(1, &m_VBO);
        glDeleteBuffers(1, &m_EBO);
    }
}

bool Model::loadOBJ(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Failed to open OBJ file: " << path << std::endl;
        return false;
    }

    std::vector<glm::vec3> temp_vertices;
    std::vector<glm::vec2> temp_uvs;
    std::vector<glm::vec3> temp_normals;

    std::string line;
    while (std::getline(file, line)) {
        if (line.substr(0, 2) == "v ") {
            std::istringstream s(line.substr(2));
            glm::vec3 v; s >> v.x >> v.y >> v.z;
            temp_vertices.push_back(v);
        } else if (line.substr(0, 3) == "vt ") {
            std::istringstream s(line.substr(3));
            glm::vec2 uv; s >> uv.x >> uv.y;
            temp_uvs.push_back(uv);
        } else if (line.substr(0, 3) == "vn ") {
            std::istringstream s(line.substr(3));
            glm::vec3 n; s >> n.x >> n.y >> n.z;
            temp_normals.push_back(n);
        } else if (line.substr(0, 2) == "f ") {
            std::istringstream s(line.substr(2));
            std::string token;
            while (s >> token) {
                std::istringstream part(token);
                std::string v, t, n;
                std::getline(part, v, '/');
                std::getline(part, t, '/');
                std::getline(part, n, '/');
                
                Vertex vertex;
                if (!v.empty()) vertex.Position = temp_vertices[std::stoi(v) - 1];
                if (!t.empty() && !temp_uvs.empty()) vertex.TexCoords = temp_uvs[std::stoi(t) - 1];
                else vertex.TexCoords = glm::vec2(0.0f);
                if (!n.empty() && !temp_normals.empty()) vertex.Normal = temp_normals[std::stoi(n) - 1];
                else vertex.Normal = glm::vec3(0.0f, 1.0f, 0.0f);
                
                m_Vertices.push_back(vertex);
                m_Indices.push_back(m_Vertices.size() - 1);
            }
        }
    }
    
    setupMesh();
    return true;
}

void Model::setupMesh() {
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);
    
    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, m_Vertices.size() * sizeof(Vertex), &m_Vertices[0], GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_Indices.size() * sizeof(unsigned int), &m_Indices[0], GL_STATIC_DRAW);
    
    // Positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    // TexCoords
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
    // Normals
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
    
    glBindVertexArray(0);
    m_IsInitialized = true;
}

void Model::draw() const {
    if (!m_IsInitialized) return;
    glBindVertexArray(m_VAO);
    glDrawElements(GL_TRIANGLES, m_Indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

} // namespace render
