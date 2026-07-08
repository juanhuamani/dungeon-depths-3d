#pragma once

#include "world/Dungeon.h"
#include "world/DungeonGenerator.h"
#include "world/DungeonLayoutBuilder.h"
#include "world/DungeonTypes.h"
#include "world/TileMap.h"

#include <optional>

namespace game {

class Level {
public:
    void generate(const world::GeneratorConfig& generatorConfig = {},
                  const world::LayoutConfig& layoutConfig = {});

    const world::TileMap& tileMap() const { return tileMap_; }
    const world::Dungeon& dungeon() const { return dungeon_; }
    std::optional<world::TileRect> contentBounds() const;

private:
    world::Dungeon dungeon_;
    world::TileMap tileMap_;
    world::DungeonGenerator generator_;
    world::DungeonLayoutBuilder layoutBuilder_;
};

} // namespace game
