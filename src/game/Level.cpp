#include "game/Level.h"

namespace game {

void Level::generate(const world::GeneratorConfig& generatorConfig,
                     const world::LayoutConfig& layoutConfig) {
    dungeon_ = generator_.generate(generatorConfig);
    tileMap_ = layoutBuilder_.build(dungeon_, layoutConfig);
}

std::optional<world::TileRect> Level::contentBounds() const {
    return tileMap_.contentBounds();
}

} // namespace game
