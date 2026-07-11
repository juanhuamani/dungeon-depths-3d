#include "render/TileColors.h"

#include "world/DungeonTypes.h"

namespace render {

namespace {

constexpr glm::vec4 kRoomWallColor{0.25f, 0.25f, 0.28f, 1.0f};
/** Darker than corridor floor (0.55) so walls remain readable while moving. */
constexpr glm::vec4 kCorridorWallColor{0.38f, 0.38f, 0.40f, 1.0f};
constexpr glm::vec4 kCorridorFloorColor{0.55f, 0.55f, 0.55f, 1.0f};

bool isRoomSideTile(world::TileType type) {
    return world::isRoomInterior(type) || type == world::TileType::Door;
}

} // namespace

glm::vec4 colorForTileType(world::TileType type) {
    switch (type) {
        case world::TileType::Room:     return {0.75f, 0.75f, 0.75f, 1.0f};
        case world::TileType::Wall:     return kRoomWallColor;
        case world::TileType::Corridor: return kCorridorFloorColor;
        case world::TileType::Door:     return {0.55f, 0.35f, 0.20f, 1.0f};
        case world::TileType::Entrance: return {0.20f, 0.80f, 0.30f, 1.0f};
        case world::TileType::Exit:     return {0.85f, 0.20f, 0.20f, 1.0f};
        case world::TileType::Treasure: return {0.90f, 0.80f, 0.20f, 1.0f};
        case world::TileType::Enemy:    return {0.70f, 0.20f, 0.70f, 1.0f};
        case world::TileType::Key:      return {0.30f, 0.60f, 0.90f, 1.0f};
        default:                        return {0.0f, 0.0f, 0.0f, 0.0f};
    }
}

glm::vec4 wallColorForNeighbors(const world::TileMap& tileMap, world::TilePos pos) {
    const world::TilePos neighbors[] = {
        {pos.row - 1, pos.col},
        {pos.row + 1, pos.col},
        {pos.row, pos.col - 1},
        {pos.row, pos.col + 1},
    };

    for (const world::TilePos& neighbor : neighbors) {
        if (isRoomSideTile(tileMap.at(neighbor))) {
            return kRoomWallColor;
        }
    }
    return kCorridorWallColor;
}

} // namespace render
