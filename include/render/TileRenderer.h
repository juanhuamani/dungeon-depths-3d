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

    void render(const glm::mat4& viewProjection, const glm::vec3& viewPos) const;

private:
    engine::Shader shader_;
    TileBatch batch_;
    float tileSize_ = 1.0f;
    bool initialized_ = false;
    std::vector<PointLight> lights_;
    std::shared_ptr<Texture2D> texture_;
};

} // namespace render
