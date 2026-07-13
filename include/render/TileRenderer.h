#pragma once

#include "render/TileBatch.h"
#include "world/TileMap.h"
#include "render/Texture2D.h"

#include "engine/Shader.h"

#include <glm/glm.hpp>
#include <string>
#include <memory>
#include <vector>

namespace render {

class TileRenderer {
public:
    bool init(const std::string& vertexShaderPath, const std::string& fragmentShaderPath);
    void shutdown();
    void setTileMap(const world::TileMap& tileMap);
    void setTileSize(float tileSize);
    void setTexture(std::shared_ptr<Texture2D> texture);
    
    struct PointLight {
        glm::vec3 position;
        glm::vec3 color;
    };
    void setLights(const std::vector<PointLight>& lights);
    
    struct DirLight {
        glm::vec3 direction;
        glm::vec3 color;
    };
    void setDirLight(const DirLight& dirLight);
    void setShadowMap(unsigned int depthMap, const glm::mat4& lightSpaceMatrix);

    void render(const glm::mat4& viewProjection, const glm::vec3& viewPos) const;
    void renderShadow(const glm::mat4& lightSpaceMatrix) const;

private:
    engine::Shader shader_;
    engine::Shader shadowShader_;
    TileBatch batch_;
    float tileSize_ = 1.0f;
    bool initialized_ = false;
    std::vector<PointLight> lights_;
    std::shared_ptr<Texture2D> texture_;
    DirLight dirLight_;
    unsigned int shadowMap_ = 0;
    glm::mat4 lightSpaceMatrix_{1.0f};
};

} // namespace render
