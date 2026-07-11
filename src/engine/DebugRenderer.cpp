#include "engine/DebugRenderer.h"
#include "world/TileMap.h"
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

bool DebugRenderer::init()
{
    if (!m_shader.loadFromFiles("assets/shaders/debug.vert", "assets/shaders/debug.frag"))
        return false;

    if (!m_crosshairShader.loadFromFiles("assets/shaders/crosshair.vert", "assets/shaders/crosshair.frag"))
        return false;

    createCubeMesh();
    createPlaneMesh();
    createCrosshairMesh();

    glGenVertexArrays(1, &m_minimapVAO);
    glGenBuffers(1, &m_minimapVBO);
    glBindVertexArray(m_minimapVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_minimapVBO);
    glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

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
    if (m_minimapVAO)   { glDeleteVertexArrays(1, &m_minimapVAO);    m_minimapVAO   = 0; }
    if (m_minimapVBO)   { glDeleteBuffers(1, &m_minimapVBO);          m_minimapVBO   = 0; }
    m_shader.destroy();
    m_crosshairShader.destroy();
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
    m_crosshairShader.setVec4("uColor", glm::vec4(color, 1.0f));

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

void DebugRenderer::drawMinimap(float screenWidth, float screenHeight,
                                 const world::TileMap& tileMap,
                                 const glm::vec3& playerPos,
                                 float playerYaw,
                                 float tileSize)
{
    float minimapWidth = 180.0f;
    float minimapHeight = 180.0f;
    float margin = 20.0f;

    float minX = screenWidth - minimapWidth - margin;
    float minY = screenHeight - minimapHeight - margin;
    float maxX = screenWidth - margin;
    float maxY = screenHeight - margin;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);

    m_crosshairShader.use();
    glm::mat4 ortho = glm::ortho(0.0f, screenWidth, 0.0f, screenHeight);
    m_crosshairShader.setMat4("projection", ortho);
    glUniform2f(glGetUniformLocation(m_crosshairShader.getID(), "uOffset"), 0.0f, 0.0f);

    auto addQuad = [](std::vector<float>& vertices, float x1, float y1, float x2, float y2) {
        vertices.push_back(x1); vertices.push_back(y1);
        vertices.push_back(x2); vertices.push_back(y1);
        vertices.push_back(x2); vertices.push_back(y2);
        vertices.push_back(x1); vertices.push_back(y1);
        vertices.push_back(x2); vertices.push_back(y2);
        vertices.push_back(x1); vertices.push_back(y2);
    };

    auto addTriangle = [](std::vector<float>& vertices, float x1, float y1, float x2, float y2, float x3, float y3) {
        vertices.push_back(x1); vertices.push_back(y1);
        vertices.push_back(x2); vertices.push_back(y2);
        vertices.push_back(x3); vertices.push_back(y3);
    };

    glBindVertexArray(m_minimapVAO);

    auto drawBatch = [&](const std::vector<float>& vertices, const glm::vec4& color) {
        if (vertices.empty()) return;
        glBindBuffer(GL_ARRAY_BUFFER, m_minimapVBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);
        m_crosshairShader.setVec4("uColor", color);
        glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(vertices.size() / 2));
    };

    std::vector<float> borderVertices;
    addQuad(borderVertices, minX - 2.0f, minY - 2.0f, maxX + 2.0f, maxY + 2.0f);
    drawBatch(borderVertices, glm::vec4(0.12f, 0.35f, 0.7f, 0.9f));

    std::vector<float> bgVertices;
    addQuad(bgVertices, minX, minY, maxX, maxY);
    drawBatch(bgVertices, glm::vec4(0.04f, 0.04f, 0.1f, 0.85f));

    glEnable(GL_SCISSOR_TEST);
    glScissor(static_cast<GLint>(minX), static_cast<GLint>(minY), static_cast<GLint>(minimapWidth), static_cast<GLint>(minimapHeight));

    const float visibleRadius = 8.0f;
    float centerX = minX + minimapWidth * 0.5f;
    float centerY = minY + minimapHeight * 0.5f;
    float scale = minimapWidth / (2.0f * visibleRadius);

    int rows = tileMap.rows();
    int cols = tileMap.cols();
    if (rows > 0 && cols > 0) {
        float pCol = playerPos.x / tileSize;
        float pRow = playerPos.z / tileSize;

        std::vector<float> wallsVec, roomsVec, corridorsVec, doorsVec, entranceVec, exitVec, treasureVec, enemiesVec, keysVec;

        for (int r = 0; r < rows; ++r) {
            for (int c = 0; c < cols; ++c) {
                world::TilePos pos{r, c};
                world::TileType type = tileMap.at(pos);
                if (type == world::TileType::Empty) continue;

                // Calculate screen position relative to player centered in the minimap
                float tileCenterX = centerX + (static_cast<float>(c) + 0.5f - pCol) * scale;
                float tileCenterY = centerY - (static_cast<float>(r) + 0.5f - pRow) * scale;

                float x1 = tileCenterX - scale * 0.5f;
                float y1 = tileCenterY - scale * 0.5f;
                float x2 = tileCenterX + scale * 0.5f;
                float y2 = tileCenterY + scale * 0.5f;

                // Grid cell border/gap (shrink a bit)
                float shrink = scale * 0.04f;
                if (shrink < 0.5f) shrink = 0.5f;
                x1 += shrink;
                y1 += shrink;
                x2 -= shrink;
                y2 -= shrink;

                switch (type) {
                    case world::TileType::Wall:
                        addQuad(wallsVec, x1, y1, x2, y2);
                        break;
                    case world::TileType::Room:
                        addQuad(roomsVec, x1, y1, x2, y2);
                        break;
                    case world::TileType::Corridor:
                        addQuad(corridorsVec, x1, y1, x2, y2);
                        break;
                    case world::TileType::Door:
                        addQuad(doorsVec, x1, y1, x2, y2);
                        break;
                    case world::TileType::Entrance:
                        addQuad(entranceVec, x1, y1, x2, y2);
                        break;
                    case world::TileType::Exit:
                        addQuad(exitVec, x1, y1, x2, y2);
                        break;
                    case world::TileType::Treasure:
                        addQuad(treasureVec, x1, y1, x2, y2);
                        break;
                    case world::TileType::Enemy:
                        addQuad(enemiesVec, x1, y1, x2, y2);
                        break;
                    case world::TileType::Key:
                        addQuad(keysVec, x1, y1, x2, y2);
                        break;
                    default:
                        break;
                }
            }
        }

        drawBatch(wallsVec, glm::vec4(0.2f, 0.22f, 0.28f, 0.9f));
        drawBatch(roomsVec, glm::vec4(0.55f, 0.58f, 0.62f, 0.9f));
        drawBatch(corridorsVec, glm::vec4(0.4f, 0.42f, 0.45f, 0.9f));
        drawBatch(doorsVec, glm::vec4(0.65f, 0.4f, 0.2f, 0.9f));
        drawBatch(entranceVec, glm::vec4(0.2f, 0.8f, 0.3f, 1.0f));
        drawBatch(exitVec, glm::vec4(0.85f, 0.2f, 0.2f, 1.0f));
        drawBatch(treasureVec, glm::vec4(0.95f, 0.75f, 0.1f, 1.0f));
        drawBatch(enemiesVec, glm::vec4(0.8f, 0.15f, 0.8f, 1.0f));
        drawBatch(keysVec, glm::vec4(0.15f, 0.65f, 0.95f, 1.0f));

        // 5. Draw player exactly at the center of the minimap
        float yawRad = glm::radians(playerYaw);
        glm::vec2 dir(cos(yawRad), -sin(yawRad));
        glm::vec2 perp(-dir.y, dir.x);

        float playerSize = 10.0f; // Fixed size at higher zoom

        glm::vec2 p1 = glm::vec2(centerX, centerY) + dir * playerSize;
        glm::vec2 p2 = glm::vec2(centerX, centerY) - dir * (playerSize * 0.5f) + perp * (playerSize * 0.5f);
        glm::vec2 p3 = glm::vec2(centerX, centerY) - dir * (playerSize * 0.5f) - perp * (playerSize * 0.5f);

        std::vector<float> playerVertices;
        addTriangle(playerVertices, p1.x, p1.y, p2.x, p2.y, p3.x, p3.y);
        drawBatch(playerVertices, glm::vec4(0.0f, 0.95f, 0.95f, 1.0f));
    }

    glDisable(GL_SCISSOR_TEST);

    glBindVertexArray(0);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
}
