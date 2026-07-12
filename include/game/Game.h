#pragma once

#include "game/Level.h"
#include "render/MapCamera.h"
#include "render/TileRenderer.h"
#include "render/TileCoordinates.h"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <string>

namespace game {

class Game {
public:
    bool init(int viewportWidth, int viewportHeight,
              const std::string& vertexShaderPath,
              const std::string& fragmentShaderPath);
    void shutdown();
    void update(float deltaTime, GLFWwindow* window);
    void render(int viewportWidth, int viewportHeight) const;
    void renderWithViewProjection(const glm::mat4& viewProjection) const;
    void onFramebufferResize(int width, int height);

    render::MapCamera& mapCamera() { return mapCamera_; }
    const Level& level() const { return level_; }

    glm::vec3 getPlayerSpawnPosition() const;
    bool isWalkable(const glm::vec3& worldPos) const;
    bool isExitTile(const glm::vec3& worldPos) const;
    glm::vec3 resolveWallCollision(const glm::vec3& position, const glm::vec3& halfSize) const;
    float getTileSize() const { return tileSize_; }

private:
    void centerCameraOnLevel();
    void processCameraInput(GLFWwindow* window, float deltaTime);
    void processViewModeInput(GLFWwindow* window);
    world::TilePos worldToTile(const glm::vec3& worldPos) const;
    bool isTileSolid(world::TilePos pos) const;

    Level level_;
    render::TileRenderer tileRenderer_;
    render::MapCamera mapCamera_;

    int viewportWidth_ = 1280;
    int viewportHeight_ = 720;
    float tileSize_ = 1.0f;
    float cameraPanSpeed_ = 20.0f;
    bool initialized_ = false;

    bool zKeyWasDown_ = false;
    bool xKeyWasDown_ = false;
};

} // namespace game
