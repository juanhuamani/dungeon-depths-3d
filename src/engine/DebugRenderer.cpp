#include "engine/DebugRenderer.h"
#include <glm/gtc/matrix_transform.hpp>

bool DebugRenderer::init()
{
    if (!m_shader.loadFromFiles("assets/shaders/debug.vert", "assets/shaders/debug.frag"))
        return false;

    if (!m_crosshairShader.loadFromFiles("assets/shaders/crosshair.vert", "assets/shaders/crosshair.frag"))
        return false;

    createCubeMesh();
    createPlaneMesh();
    createCrosshairMesh();
    return true;
}

void DebugRenderer::cleanup()
{
    if (m_cubeVAO)      { glDeleteVertexArrays(1, &m_cubeVAO);      m_cubeVAO      = 0; }
    if (m_cubeVBO)      { glDeleteBuffers(1, &m_cubeVBO);            m_cubeVBO      = 0; }
    if (m_planeVAO)     { glDeleteVertexArrays(1, &m_planeVAO);      m_planeVAO     = 0; }
    if (m_planeVBO)     { glDeleteBuffers(1, &m_planeVBO);            m_planeVBO     = 0; }
    if (m_crosshairVAO) { glDeleteVertexArrays(1, &m_crosshairVAO);  m_crosshairVAO = 0; }
    if (m_crosshairVBO) { glDeleteBuffers(1, &m_crosshairVBO);        m_crosshairVBO = 0; }
}

void DebugRenderer::setViewProjection(const glm::mat4& view, const glm::mat4& projection)
{
    m_view       = view;
    m_projection = projection;
}

void DebugRenderer::drawCube(const glm::vec3& position, const glm::vec3& scale, const glm::vec3& color)
{
    glm::mat4 model{1.0f};
    model = glm::translate(model, position);
    model = glm::scale(model, scale);
    drawCube(model, color);
}

void DebugRenderer::drawCube(const glm::mat4& model, const glm::vec3& color)
{
    m_shader.use();
    m_shader.setMat4("model", model);
    m_shader.setMat4("view", m_view);
    m_shader.setMat4("projection", m_projection);
    m_shader.setVec3("uColor", color);

    glBindVertexArray(m_cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

void DebugRenderer::drawGround(float size, const glm::vec3& color)
{
    glm::mat4 model{1.0f};
    model = glm::scale(model, glm::vec3(size, 1.0f, size));

    m_shader.use();
    m_shader.setMat4("model", model);
    m_shader.setMat4("view", m_view);
    m_shader.setMat4("projection", m_projection);
    m_shader.setVec3("uColor", color);

    glBindVertexArray(m_planeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void DebugRenderer::drawCrosshair(float screenWidth, float screenHeight, float size, const glm::vec3& color)
{
    glm::mat4 ortho = glm::ortho(0.0f, screenWidth, 0.0f, screenHeight);
    glm::vec2 center(screenWidth * 0.5f, screenHeight * 0.5f);

    glDisable(GL_DEPTH_TEST);

    m_crosshairShader.use();
    m_crosshairShader.setMat4("projection", ortho);
    m_crosshairShader.setVec3("uColor", color);

    float half = size * 0.5f;
    float thick = 1.5f;

    float lines[] = {
        center.x - half, center.y - thick,
        center.x + half, center.y - thick,
        center.x + half, center.y + thick,
        center.x + half, center.y + thick,
        center.x - half, center.y + thick,
        center.x - half, center.y - thick,

        center.x - thick, center.y - half,
        center.x + thick, center.y - half,
        center.x + thick, center.y + half,
        center.x + thick, center.y + half,
        center.x - thick, center.y + half,
        center.x - thick, center.y - half,

        center.x - 2.0f, center.y - 2.0f,
        center.x + 2.0f, center.y - 2.0f,
        center.x + 2.0f, center.y + 2.0f,
        center.x + 2.0f, center.y + 2.0f,
        center.x - 2.0f, center.y + 2.0f,
        center.x - 2.0f, center.y - 2.0f,
    };

    glBindVertexArray(m_crosshairVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_crosshairVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(lines), lines);

    glUniform2f(glGetUniformLocation(m_crosshairShader.getID(), "uOffset"), 0.0f, 0.0f);
    glDrawArrays(GL_TRIANGLES, 0, 18);

    glBindVertexArray(0);
    glEnable(GL_DEPTH_TEST);
}

void DebugRenderer::createCubeMesh()
{
    float vertices[] = {
        -0.5f, -0.5f,  0.5f,   0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,   0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,   0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,   0.0f,  0.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,   0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,   0.0f,  0.0f,  1.0f,

        -0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,   0.0f,  0.0f, -1.0f,
         0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,   0.0f,  0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,   0.0f,  0.0f, -1.0f,

        -0.5f,  0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,

         0.5f,  0.5f,  0.5f,   1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,   1.0f,  0.0f,  0.0f,
         0.5f,  0.5f, -0.5f,   1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,   1.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,   1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,   1.0f,  0.0f,  0.0f,

        -0.5f,  0.5f, -0.5f,   0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,   0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,   0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,   0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,   0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,   0.0f,  1.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,   0.0f, -1.0f,  0.0f,
         0.5f, -0.5f, -0.5f,   0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,   0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,   0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,   0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,   0.0f, -1.0f,  0.0f,
    };

    glGenVertexArrays(1, &m_cubeVAO);
    glGenBuffers(1, &m_cubeVBO);

    glBindVertexArray(m_cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void DebugRenderer::createPlaneMesh()
{
    float vertices[] = {
        -0.5f, 0.0f, -0.5f,     0.0f, 1.0f, 0.0f,
         0.5f, 0.0f, -0.5f,     0.0f, 1.0f, 0.0f,
         0.5f, 0.0f,  0.5f,     0.0f, 1.0f, 0.0f,
         0.5f, 0.0f,  0.5f,     0.0f, 1.0f, 0.0f,
        -0.5f, 0.0f,  0.5f,     0.0f, 1.0f, 0.0f,
        -0.5f, 0.0f, -0.5f,     0.0f, 1.0f, 0.0f,
    };

    glGenVertexArrays(1, &m_planeVAO);
    glGenBuffers(1, &m_planeVBO);

    glBindVertexArray(m_planeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void DebugRenderer::createCrosshairMesh()
{
    glGenVertexArrays(1, &m_crosshairVAO);
    glGenBuffers(1, &m_crosshairVBO);

    glBindVertexArray(m_crosshairVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_crosshairVBO);
    glBufferData(GL_ARRAY_BUFFER, 18 * 2 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}
