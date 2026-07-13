#include "ui/UIBatch.h"
#include <iostream>
#include <cstddef>

UIBatch::UIBatch() {}

UIBatch::~UIBatch() {
    shutdown();
}

bool UIBatch::init() {
    if (!m_shader.loadFromFiles("assets/shaders/ui.vert", "assets/shaders/ui.frag")) {
        std::cerr << "UIBatch: Failed to load UI shaders\n";
        return false;
    }

    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    glGenBuffers(1, &m_ebo);

    glBindVertexArray(m_vao);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, MAX_VERTICES * sizeof(UIVertex), nullptr, GL_DYNAMIC_DRAW);

    // Indices
    std::vector<GLuint> indices(MAX_INDICES);
    GLuint offset = 0;
    for (int i = 0; i < MAX_INDICES; i += 6) {
        indices[i + 0] = offset + 0;
        indices[i + 1] = offset + 1;
        indices[i + 2] = offset + 2;
        indices[i + 3] = offset + 2;
        indices[i + 4] = offset + 3;
        indices[i + 5] = offset + 0;
        offset += 4;
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

    // Attributes
    // Position (vec2)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(UIVertex), (void*)offsetof(UIVertex, position));
    // TexCoords (vec2)
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(UIVertex), (void*)offsetof(UIVertex, texCoords));
    // Color (vec4)
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(UIVertex), (void*)offsetof(UIVertex, color));
    // TexIndex (float)
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(UIVertex), (void*)offsetof(UIVertex, texIndex));

    glBindVertexArray(0);

    m_vertices.reserve(MAX_VERTICES);

    m_shader.use();
    for (int i = 0; i < MAX_TEXTURE_SLOTS; i++) {
        m_shader.setInt("uTextures[" + std::to_string(i) + "]", i);
    }

    return true;
}

void UIBatch::shutdown() {
    if (m_vao) { glDeleteVertexArrays(1, &m_vao); m_vao = 0; }
    if (m_vbo) { glDeleteBuffers(1, &m_vbo); m_vbo = 0; }
    if (m_ebo) { glDeleteBuffers(1, &m_ebo); m_ebo = 0; }
    m_shader.destroy();
}

void UIBatch::begin(const glm::mat4& projection) {
    m_projection = projection;
    m_vertices.clear();
    m_textureCount = 0;
}

int UIBatch::getTextureSlot(GLuint textureId) {
    if (textureId == 0) return -1;

    for (int i = 0; i < m_textureCount; i++) {
        if (m_textures[i] == textureId) {
            return i;
        }
    }

    if (m_textureCount >= MAX_TEXTURE_SLOTS) {
        flush();
    }

    m_textures[m_textureCount] = textureId;
    return m_textureCount++;
}

void UIBatch::draw(float x, float y, float w, float h, GLuint textureId, const glm::vec4& color) {
    if (m_vertices.size() >= MAX_VERTICES) {
        flush();
    }

    float texIndex = (float)getTextureSlot(textureId);

    // Top Left
    m_vertices.push_back({{x, y}, {0.0f, 1.0f}, color, texIndex});
    // Top Right
    m_vertices.push_back({{x + w, y}, {1.0f, 1.0f}, color, texIndex});
    // Bottom Right
    m_vertices.push_back({{x + w, y + h}, {1.0f, 0.0f}, color, texIndex});
    // Bottom Left
    m_vertices.push_back({{x, y + h}, {0.0f, 0.0f}, color, texIndex});
}

void UIBatch::end() {
    flush();
}

void UIBatch::flush() {
    if (m_vertices.empty()) return;

    m_shader.use();
    m_shader.setMat4("projection", m_projection);

    for (int i = 0; i < m_textureCount; i++) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, m_textures[i]);
    }

    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, m_vertices.size() * sizeof(UIVertex), m_vertices.data());

    int indexCount = (m_vertices.size() / 4) * 6;
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);

    m_vertices.clear();
    m_textureCount = 0;
}
