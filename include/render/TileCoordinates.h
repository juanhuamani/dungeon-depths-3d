#pragma once

#include "world/DungeonTypes.h"

#include <glm/glm.hpp>

namespace render {

glm::vec2 tileToWorld(world::TilePos pos, float tileSize);
glm::vec2 tileRectCenterWorld(const world::TileRect& rect, float tileSize);

} // namespace render
