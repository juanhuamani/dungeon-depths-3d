#pragma once

#include "render/TileBatch.h"
#include "world/TileMap.h"

#include "engine/Shader.h"

#include <glm/glm.hpp>
#include <string>

namespace render {

class TileRenderer {
public:
    bool init(const std::string& vertexShaderPath, const std::string& fragmentShaderPath);
    void shutdown();
    void setTileMap(const world::TileMap& tileMap);
    void setTileSize(float tileSize);
    void render(const glm::mat4& viewProjection) const;

private:
    engine::Shader shader_;
    TileBatch batch_;
    float tileSize_ = 1.0f;
    bool initialized_ = false;
};

} // namespace render
