#pragma once

#include "engine/ShaderProgram.h"
#include <glad/gl.h>
#include <glm/glm.hpp>
#include <vector>

struct UIVertex {
    glm::vec2 position;
    glm::vec2 texCoords;
    glm::vec4 color;
    float texIndex;
};

class UIBatch {
public:
    UIBatch();
    ~UIBatch();

    bool init();
    void shutdown();

    void begin(const glm::mat4& projection);
    void draw(float x, float y, float w, float h, GLuint textureId, const glm::vec4& color = glm::vec4(1.0f));
    void end();

private:
    void flush();
    int getTextureSlot(GLuint textureId);

    static const int MAX_QUADS = 1000;
    static const int MAX_VERTICES = MAX_QUADS * 4;
    static const int MAX_INDICES = MAX_QUADS * 6;
    static const int MAX_TEXTURE_SLOTS = 8;

    ShaderProgram m_shader;
    GLuint m_vao = 0;
    GLuint m_vbo = 0;
    GLuint m_ebo = 0;

    std::vector<UIVertex> m_vertices;
    GLuint m_textures[MAX_TEXTURE_SLOTS];
    int m_textureCount = 0;
    
    glm::mat4 m_projection;
};
