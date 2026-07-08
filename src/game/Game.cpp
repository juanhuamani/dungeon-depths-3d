#include "game/Game.h"

#include "render/TileCoordinates.h"

#include <algorithm>
#include <iostream>

namespace game {

namespace {

bool isKeyDown(GLFWwindow* window, int key) {
    return glfwGetKey(window, key) == GLFW_PRESS;
}

} // namespace

bool Game::init(int viewportWidth, int viewportHeight,
                const std::string& vertexShaderPath,
                const std::string& fragmentShaderPath) {
    viewportWidth_ = viewportWidth;
    viewportHeight_ = viewportHeight;

    if (!tileRenderer_.init(vertexShaderPath, fragmentShaderPath)) {
        std::cerr << "Error: no se pudo inicializar TileRenderer\n";
        return false;
    }

    world::GeneratorConfig generatorConfig;
    generatorConfig.rows = 12;
    generatorConfig.cols = 16;
    generatorConfig.mainPathMinLength = 6;
    generatorConfig.mainPathMaxLength = 10;
    generatorConfig.maxBranchLength = 3;
    generatorConfig.seed = 0;

    world::LayoutConfig layoutConfig;
    layoutConfig.corridorWidth = 3;
    layoutConfig.seed = 0;

    level_.generate(generatorConfig, layoutConfig);

    camera_.setTileSize(tileSize_);
    tileRenderer_.setTileSize(tileSize_);
    tileRenderer_.setTileMap(level_.tileMap());
    centerCameraOnLevel();

    initialized_ = true;
    return true;
}

void Game::centerCameraOnLevel() {
    const std::optional<world::TileRect> bounds = level_.contentBounds();
    if (!bounds.has_value()) {
        camera_.centerOn({0.0f, 0.0f});
        return;
    }

    const glm::vec2 center = render::tileRectCenterWorld(bounds.value(), tileSize_);
    camera_.centerOn(center);

    const float mapWidth = static_cast<float>(bounds->width) * tileSize_;
    const float mapHeight = static_cast<float>(bounds->height) * tileSize_;
    const float aspect = static_cast<float>(viewportWidth_) / static_cast<float>(viewportHeight_);
    const float baseHeight = 40.0f;

    const float zoomForHeight = baseHeight / std::max(mapHeight * 1.4f, 1.0f);
    const float zoomForWidth = (baseHeight * aspect) / std::max(mapWidth * 1.4f, 1.0f);
    camera_.setZoom(std::min(zoomForHeight, zoomForWidth));
}

void Game::onFramebufferResize(int width, int height) {
    viewportWidth_ = std::max(width, 1);
    viewportHeight_ = std::max(height, 1);
}

void Game::processCameraInput(GLFWwindow* window, float deltaTime) {
    glm::vec2 panDelta{0.0f, 0.0f};
    const float speed = cameraPanSpeed_ * deltaTime / camera_.zoom();

    if (isKeyDown(window, GLFW_KEY_W) || isKeyDown(window, GLFW_KEY_UP)) {
        panDelta.y += speed;
    }
    if (isKeyDown(window, GLFW_KEY_S) || isKeyDown(window, GLFW_KEY_DOWN)) {
        panDelta.y -= speed;
    }
    if (isKeyDown(window, GLFW_KEY_A) || isKeyDown(window, GLFW_KEY_LEFT)) {
        panDelta.x -= speed;
    }
    if (isKeyDown(window, GLFW_KEY_D) || isKeyDown(window, GLFW_KEY_RIGHT)) {
        panDelta.x += speed;
    }

    if (isKeyDown(window, GLFW_KEY_EQUAL) || isKeyDown(window, GLFW_KEY_KP_ADD)) {
        camera_.zoomBy(1.0f + deltaTime * 2.0f);
    }
    if (isKeyDown(window, GLFW_KEY_MINUS) || isKeyDown(window, GLFW_KEY_KP_SUBTRACT)) {
        camera_.zoomBy(1.0f / (1.0f + deltaTime * 2.0f));
    }

    camera_.pan(panDelta);
}

void Game::update(float deltaTime, GLFWwindow* window) {
    if (!initialized_) {
        return;
    }
    processCameraInput(window, deltaTime);
}

void Game::render(int viewportWidth, int viewportHeight) const {
    if (!initialized_) {
        return;
    }
    tileRenderer_.render(camera_, static_cast<float>(viewportWidth), static_cast<float>(viewportHeight));
}

} // namespace game
