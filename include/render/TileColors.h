#pragma once

#include "world/DungeonTypes.h"

#include <glm/glm.hpp>

namespace render {

glm::vec4 colorForTileType(world::TileType type);

} // namespace render
