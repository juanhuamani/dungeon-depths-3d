#include "render/TileCoordinates.h"

namespace render {

glm::vec3 tileOrigin(world::TilePos pos, float tileSize) {
    return {
        static_cast<float>(pos.col) * tileSize,
        0.0f,
        static_cast<float>(pos.row) * tileSize
    };
}

glm::vec2 tileRectCenterXZ(const world::TileRect& rect, float tileSize) {
    const float centerCol = static_cast<float>(rect.col) + static_cast<float>(rect.width) * 0.5f;
    const float centerRow = static_cast<float>(rect.row) + static_cast<float>(rect.height) * 0.5f;
    return {
        centerCol * tileSize,
        centerRow * tileSize
    };
}

} // namespace render
