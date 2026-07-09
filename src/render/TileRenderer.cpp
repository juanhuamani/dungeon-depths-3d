#include "render/TileRenderer.h"

namespace render {

bool TileRenderer::init(const std::string& vertexShaderPath, const std::string& fragmentShaderPath) {
    initialized_ = shader_.loadFromFiles(vertexShaderPath, fragmentShaderPath);
    return initialized_;
}

void TileRenderer::shutdown() {
    batch_.destroyGpu();
    shader_.destroy();
    initialized_ = false;
}

void TileRenderer::setTileSize(float tileSize) {
    tileSize_ = tileSize;
}

void TileRenderer::setTileMap(const world::TileMap& tileMap) {
    batch_.buildFromTileMap(tileMap, tileSize_);
}

void TileRenderer::render(const glm::mat4& viewProjection) const {
    if (!initialized_ || batch_.isEmpty()) {
        return;
    }

    shader_.use();
    shader_.setMat4("uViewProjection", viewProjection);
    batch_.draw();
}

} // namespace render
