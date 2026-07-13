#include "render/TileRenderer.h"

namespace render {

bool TileRenderer::init(const std::string& vertexShaderPath, const std::string& fragmentShaderPath) {
    if (!shader_.loadFromFiles(vertexShaderPath, fragmentShaderPath)) {
        return false;
    }
    if (!shadowShader_.loadFromFiles("assets/shaders/shadow.vert", "assets/shaders/shadow.frag")) {
        return false;
    }
    initialized_ = true;
    return true;
}

void TileRenderer::shutdown() {
    batch_.destroyGpu();
    shader_.destroy();
    shadowShader_.destroy();
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

void TileRenderer::setDirLight(const DirLight& dirLight) {
    dirLight_ = dirLight;
}

void TileRenderer::setShadowMap(unsigned int depthMap, const glm::mat4& lightSpaceMatrix) {
    shadowMap_ = depthMap;
    lightSpaceMatrix_ = lightSpaceMatrix;
}

void TileRenderer::setTexture(std::shared_ptr<Texture2D> texture) {
    texture_ = texture;
}

void TileRenderer::renderShadow(const glm::mat4& lightSpaceMatrix) const {
    if (!initialized_ || batch_.isEmpty()) return;
    
    shadowShader_.use();
    shadowShader_.setMat4("lightSpaceMatrix", lightSpaceMatrix);
    shadowShader_.setMat4("model", glm::mat4(1.0f));
    
    batch_.draw();
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
    
    // Set directional light
    shader_.setVec3("dirLightDir", dirLight_.direction);
    shader_.setVec3("dirLightColor", dirLight_.color);
    shader_.setMat4("lightSpaceMatrix", lightSpaceMatrix_);

    // Set shadow map
    if (shadowMap_ != 0) {
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, shadowMap_);
        shader_.setInt("shadowMap", 1);
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
