#pragma once

#include "world/Dungeon.h"
#include "world/DungeonTypes.h"

#include <random>
#include <optional>
#include <vector>

namespace world {

class DungeonGenerator {
public:
    Dungeon generate(const GeneratorConfig& config = {});

private:
    void stage1_initGrid(Dungeon& dungeon);
    bool stage2_placeEntrance(Dungeon& dungeon);
    bool stage3_buildMainPath(Dungeon& dungeon);
    void stage4_addBranches(Dungeon& dungeon);
    void stage5_addExtraConnections(Dungeon& dungeon);
    bool stage6_validate(const Dungeon& dungeon) const;

    std::optional<GridPos> pickRandomBorderCell(const Dungeon& dungeon) const;
    std::optional<GridPos> pickRandomFreeNeighbor(const Dungeon& dungeon, GridPos pos) const;
    std::optional<GridPos> pickOppositeBorderTarget(const Dungeon& dungeon, GridPos entrancePos) const;
    bool isOnBorder(const Dungeon& dungeon, GridPos pos) const;
    int manhattanDistance(GridPos a, GridPos b) const;

    Room& createRoomWithRandomSize(Dungeon& dungeon, GridPos pos, RoomType type);
    bool tryConnectAdjacentRooms(Dungeon& dungeon, RoomId roomA, RoomId roomB);

    bool chance(float probability);
    int randomInt(int min, int max);
    GridPos randomChoice(const std::vector<GridPos>& options) const;

    GeneratorConfig config_{};
    mutable std::mt19937 rng_;
    std::vector<RoomId> mainPath_;
};

} // namespace world
