#pragma once

#include "engine/ShaderProgram.h"
#include <glad/gl.h>
#include <glm/glm.hpp>

namespace world {
    class TileMap;
}

class DebugRenderer {
public:
    bool init();
    void cleanup();

    void setViewProjection(const glm::mat4& view, const glm::mat4& projection);

    void drawCube(const glm::vec3& position, const glm::vec3& scale, const glm::vec3& color);
    void drawCube(const glm::mat4& model, const glm::vec3& color);
    void drawGround(float size, const glm::vec3& color);
    void drawCrosshair(float screenWidth, float screenHeight, float size, const glm::vec3& color);
    void drawMinimap(float screenWidth, float screenHeight,
                     const world::TileMap& tileMap,
                     const glm::vec3& playerPos,
                     float playerYaw,
                     float tileSize);
    void drawDamageFlash(float screenWidth, float screenHeight, float alpha);

private:
    ShaderProgram m_shader;
    ShaderProgram m_crosshairShader;
    GLuint m_cubeVAO = 0, m_cubeVBO = 0;
    GLuint m_planeVAO = 0, m_planeVBO = 0;
    GLuint m_crosshairVAO = 0, m_crosshairVBO = 0;
    GLuint m_minimapVAO = 0, m_minimapVBO = 0;

    glm::mat4 m_view{1.0f};
    glm::mat4 m_projection{1.0f};

    void createCubeMesh();
    void createPlaneMesh();
    void createCrosshairMesh();
};
