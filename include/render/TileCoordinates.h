#pragma once

#include "world/DungeonTypes.h"

#include <glm/glm.hpp>

namespace render {

namespace TileMeshConfig {
    constexpr float kFloorHeight = 0.12f;
    constexpr float kWallHeight = 2.5f;
    constexpr float kDoorHeight = 2.0f;
    constexpr float kDoorPillarRatio = 0.18f;
}

glm::vec3 tileOrigin(world::TilePos pos, float tileSize);
glm::vec2 tileRectCenterXZ(const world::TileRect& rect, float tileSize);

} // namespace render
