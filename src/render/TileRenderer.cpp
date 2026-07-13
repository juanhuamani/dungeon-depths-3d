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

void TileRenderer::setLights(const std::vector<PointLight>& lights) {
    lights_ = lights;
}

void TileRenderer::setTexture(std::shared_ptr<Texture2D> texture) {
    texture_ = texture;
}

void TileRenderer::render(const glm::mat4& viewProjection, const glm::vec3& viewPos) const {
    if (!initialized_ || batch_.isEmpty()) {
        return;
    }

    shader_.use();
    shader_.setMat4("uViewProjection", viewProjection);
    shader_.setVec3("viewPos", viewPos);
    
    // Set lights (up to 32)
    int numLights = std::min((int)lights_.size(), 32);
    shader_.setInt("numLights", numLights);
    for (int i = 0; i < numLights; ++i) {
        std::string base = "pointLights[" + std::to_string(i) + "]";
        shader_.setVec3(base + ".position", lights_[i].position);
        shader_.setVec3(base + ".color", lights_[i].color);
        shader_.setFloat(base + ".constant", 1.0f);
        shader_.setFloat(base + ".linear", 0.09f);
        shader_.setFloat(base + ".quadratic", 0.032f);
    }
    
    if (texture_) {
        texture_->bind(0);
        shader_.setInt("diffuseTex", 0);
        shader_.setInt("useTexture", 1);
    } else {
        shader_.setInt("useTexture", 0);
    }

    batch_.draw();
}

} // namespace render
