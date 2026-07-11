#pragma once

#include "world/Dungeon.h"
#include "world/DungeonTypes.h"
#include "world/TileMap.h"

#include <random>
#include <optional>

namespace world {

class DungeonLayoutBuilder {
public:
    TileMap build(Dungeon& dungeon, const LayoutConfig& config = {});

private:
    std::pair<int, int> calcMapDimensions(const Dungeon& dungeon) const;
    TilePos logicalToAnchor(GridPos gridPos) const;

    bool placeRooms(Dungeon& dungeon, TileMap& tileMap);
    bool placeDoors(Dungeon& dungeon, TileMap& tileMap);
    bool carveCorridors(Dungeon& dungeon, TileMap& tileMap);
    void sealRoomsAndCorridorWalls(Dungeon& dungeon, TileMap& tileMap);
    bool validateTileMap(const Dungeon& dungeon, const TileMap& tileMap) const;

    std::optional<Direction> getConnectionDirection(const Room& from, const Room& to) const;
    std::optional<TilePos> findDoorPosition(const TileRect& bounds, Direction dir, int usedOffset) const;
    TilePos computeExitTile(TilePos doorPos, Direction dir) const;
    TileType roomTypeToTileType(RoomType type) const;

    bool tryCarveCorridor(TileMap& tileMap, Corridor& corridor,
                          TilePos start, TilePos end, int width);

    int randomInt(int min, int max);
    bool randomBool();

    LayoutConfig config_{};
    std::mt19937 rng_;
};

} // namespace world
