#include "game/Game.h"

#include "render/TileCoordinates.h"

#include <algorithm>
#include <iostream>
#include <cmath>

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

    mapCamera_.camera2D().setTileSize(tileSize_);
    tileRenderer_.setTileSize(tileSize_);
    tileRenderer_.setTileMap(level_.tileMap());
    centerCameraOnLevel();

    initialized_ = true;
    return true;
}

void Game::shutdown() {
    if (!initialized_) return;
    tileRenderer_.shutdown();
    initialized_ = false;
}

void Game::centerCameraOnLevel() {
    const std::optional<world::TileRect> bounds = level_.contentBounds();
    if (!bounds.has_value()) {
        mapCamera_.centerOn({0.0f, 0.0f});
        return;
    }

    const glm::vec2 center = render::tileRectCenterXZ(bounds.value(), tileSize_);
    const float mapWidth = static_cast<float>(bounds->width) * tileSize_;
    const float mapHeight = static_cast<float>(bounds->height) * tileSize_;
    mapCamera_.fitToMap(center, mapWidth, mapHeight,
                      static_cast<float>(viewportWidth_),
                      static_cast<float>(viewportHeight_));
}

void Game::onFramebufferResize(int width, int height) {
    viewportWidth_ = std::max(width, 1);
    viewportHeight_ = std::max(height, 1);
}

void Game::processViewModeInput(GLFWwindow* window) {
    const bool zDown = isKeyDown(window, GLFW_KEY_Z);
    const bool xDown = isKeyDown(window, GLFW_KEY_X);

    if (zDown && !zKeyWasDown_) {
        mapCamera_.setMode(render::ViewMode::TopDown);
    }
    if (xDown && !xKeyWasDown_) {
        mapCamera_.setMode(render::ViewMode::Perspective3D);
    }

    zKeyWasDown_ = zDown;
    xKeyWasDown_ = xDown;
}

void Game::processCameraInput(GLFWwindow*, float) {
}

void Game::update(float deltaTime, GLFWwindow* window) {
    if (!initialized_) return;
    processViewModeInput(window);
}

void Game::render(int viewportWidth, int viewportHeight) const {
    if (!initialized_) return;

    const glm::mat4 viewProjection = mapCamera_.viewProjection(
        static_cast<float>(viewportWidth),
        static_cast<float>(viewportHeight));

    tileRenderer_.render(viewProjection);
}

void Game::renderWithViewProjection(const glm::mat4& viewProjection) const {
    if (!initialized_) return;
    tileRenderer_.render(viewProjection);
}

glm::vec3 Game::getPlayerSpawnPosition() const {
    auto entranceTiles = level_.tileMap().findAllTilesOfType(world::TileType::Entrance);
    if (entranceTiles.empty()) {
        auto roomTiles = level_.tileMap().findAllTilesOfType(world::TileType::Room);
        if (roomTiles.empty()) {
            return glm::vec3(0.0f, 0.0f, 0.0f);
        }
        const world::TilePos& pos = roomTiles[0];
        return glm::vec3(
            (static_cast<float>(pos.col) + 0.5f) * tileSize_,
            0.0f,
            (static_cast<float>(pos.row) + 0.5f) * tileSize_
        );
    }

    const world::TilePos& pos = entranceTiles[0];
    return glm::vec3(
        (static_cast<float>(pos.col) + 0.5f) * tileSize_,
        0.0f,
        (static_cast<float>(pos.row) + 0.5f) * tileSize_
    );
}

world::TilePos Game::worldToTile(const glm::vec3& worldPos) const {
    return {
        static_cast<int>(std::floor(worldPos.z / tileSize_)),
        static_cast<int>(std::floor(worldPos.x / tileSize_))
    };
}

bool Game::isTileSolid(world::TilePos pos) const {
    const world::TileMap& tm = level_.tileMap();
    if (!tm.isInBounds(pos)) return true;
    world::TileType type = tm.at(pos);
    return type == world::TileType::Wall || type == world::TileType::Empty;
}

bool Game::isWalkable(const glm::vec3& worldPos) const {
    return !isTileSolid(worldToTile(worldPos));
}

glm::vec3 Game::resolveWallCollision(const glm::vec3& position, const glm::vec3& halfSize) const {
    glm::vec3 resolved = position;
    const float pad = 0.05f;

    float rightEdge = resolved.x + halfSize.x;
    float leftEdge = resolved.x - halfSize.x;
    float frontEdge = resolved.z + halfSize.z;
    float backEdge = resolved.z - halfSize.z;

    auto checkSolid = [&](float x, float z) -> bool {
        return isTileSolid(worldToTile(glm::vec3(x, 0.0f, z)));
    };

    if (checkSolid(rightEdge, resolved.z) ||
        checkSolid(rightEdge, backEdge) ||
        checkSolid(rightEdge, frontEdge)) {
        int tileCol = static_cast<int>(std::floor(rightEdge / tileSize_));
        float wallLeft = static_cast<float>(tileCol) * tileSize_;
        resolved.x = wallLeft - halfSize.x - pad;
    }

    leftEdge = resolved.x - halfSize.x;
    if (checkSolid(leftEdge, resolved.z) ||
        checkSolid(leftEdge, resolved.z - halfSize.z) ||
        checkSolid(leftEdge, resolved.z + halfSize.z)) {
        int tileCol = static_cast<int>(std::floor(leftEdge / tileSize_));
        float wallRight = (static_cast<float>(tileCol) + 1.0f) * tileSize_;
        resolved.x = wallRight + halfSize.x + pad;
    }

    rightEdge = resolved.x + halfSize.x;
    leftEdge = resolved.x - halfSize.x;

    if (checkSolid(resolved.x, frontEdge) ||
        checkSolid(leftEdge, frontEdge) ||
        checkSolid(rightEdge, frontEdge)) {
        int tileRow = static_cast<int>(std::floor(frontEdge / tileSize_));
        float wallBack = static_cast<float>(tileRow) * tileSize_;
        resolved.z = wallBack - halfSize.z - pad;
    }

    frontEdge = resolved.z + halfSize.z;
    backEdge = resolved.z - halfSize.z;
    if (checkSolid(resolved.x, backEdge) ||
        checkSolid(leftEdge, backEdge) ||
        checkSolid(rightEdge, backEdge)) {
        int tileRow = static_cast<int>(std::floor(backEdge / tileSize_));
        float wallFront = (static_cast<float>(tileRow) + 1.0f) * tileSize_;
        resolved.z = wallFront + halfSize.z + pad;
    }

    return resolved;
}

} // namespace game
