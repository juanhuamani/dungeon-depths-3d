#pragma once

#include "world/DungeonTypes.h"
#include "world/TileMap.h"

#include <glm/glm.hpp>

namespace render {

glm::vec4 colorForTileType(world::TileType type);

/** Room-adjacent walls stay dark; corridor-edge walls match corridor floor gray. */
glm::vec4 wallColorForNeighbors(const world::TileMap& tileMap, world::TilePos pos);

} // namespace render
