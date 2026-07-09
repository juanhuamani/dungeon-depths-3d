#include "render/TileColors.h"

namespace render {

glm::vec4 colorForTileType(world::TileType type) {
    switch (type) {
        case world::TileType::Room:     return {0.75f, 0.75f, 0.75f, 1.0f};
        case world::TileType::Wall:     return {0.25f, 0.25f, 0.28f, 1.0f};
        case world::TileType::Corridor: return {0.55f, 0.55f, 0.55f, 1.0f};
        case world::TileType::Door:     return {0.55f, 0.35f, 0.20f, 1.0f};
        case world::TileType::Entrance: return {0.20f, 0.80f, 0.30f, 1.0f};
        case world::TileType::Exit:     return {0.85f, 0.20f, 0.20f, 1.0f};
        case world::TileType::Treasure: return {0.90f, 0.80f, 0.20f, 1.0f};
        case world::TileType::Enemy:    return {0.70f, 0.20f, 0.70f, 1.0f};
        case world::TileType::Key:      return {0.30f, 0.60f, 0.90f, 1.0f};
        default:                        return {0.0f, 0.0f, 0.0f, 0.0f};
    }
}

} // namespace render
