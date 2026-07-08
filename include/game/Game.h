#pragma once

#include "game/Level.h"
#include "render/Camera2D.h"
#include "render/TileRenderer.h"

#include <GLFW/glfw3.h>
#include <string>

namespace game {

class Game {
public:
    bool init(int viewportWidth, int viewportHeight,
              const std::string& vertexShaderPath,
              const std::string& fragmentShaderPath);
    void update(float deltaTime, GLFWwindow* window);
    void render(int viewportWidth, int viewportHeight) const;

    void onFramebufferResize(int width, int height);

    render::Camera2D& camera() { return camera_; }
    const Level& level() const { return level_; }

private:
    void centerCameraOnLevel();
    void processCameraInput(GLFWwindow* window, float deltaTime);

    Level level_;
    render::TileRenderer tileRenderer_;
    render::Camera2D camera_;

    int viewportWidth_ = 1280;
    int viewportHeight_ = 720;
    float tileSize_ = 1.0f;
    float cameraPanSpeed_ = 20.0f;
    bool initialized_ = false;
};

} // namespace game
