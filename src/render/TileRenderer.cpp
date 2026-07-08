#include "render/TileRenderer.h"

namespace render {

bool TileRenderer::init(const std::string& vertexShaderPath, const std::string& fragmentShaderPath) {
    initialized_ = shader_.loadFromFiles(vertexShaderPath, fragmentShaderPath);
    return initialized_;
}

void TileRenderer::setTileSize(float tileSize) {
    tileSize_ = tileSize;
}

void TileRenderer::setTileMap(const world::TileMap& tileMap) {
    batch_.buildFromTileMap(tileMap, tileSize_);
}

void TileRenderer::render(const Camera2D& camera, float viewportWidth, float viewportHeight) const {
    if (!initialized_ || batch_.isEmpty()) {
        return;
    }

    shader_.use();
    shader_.setMat4("uViewProjection", camera.viewProjection(viewportWidth, viewportHeight));
    batch_.draw();
}

} // namespace render
