#pragma once

#include "world/DungeonTypes.h"

#include <optional>
#include <vector>

namespace world {

class TileMap {
public:
    TileMap() = default;
    TileMap(int rows, int cols);

    int rows() const { return rows_; }
    int cols() const { return cols_; }

    TileType at(TilePos pos) const;
    void set(TilePos pos, TileType type);

    bool isInBounds(TilePos pos) const;
    bool canCarve(TilePos pos) const;

    void carveRoom(const TileRect& rect, TileType interiorType);
    bool carveCorridorPath(const std::vector<TilePos>& path, int width);
    bool carveCorridorBetween(TilePos start, TilePos end, int width, bool horizontalFirst);
    std::vector<TilePos> buildLPath(TilePos start, TilePos end, bool horizontalFirst) const;

    std::vector<TilePos> getWalkableNeighbors(TilePos pos) const;
    std::vector<TilePos> findAllTilesOfType(TileType type) const;
    bool isReachable(TilePos from, TilePos to) const;
    bool allRoomTilesReachable(TilePos from) const;
    std::optional<TileRect> contentBounds() const;

private:
    void carveWallRing(const TileRect& rect);
    bool carveThickPoint(TilePos center, int width);

    int rows_ = 0;
    int cols_ = 0;
    std::vector<std::vector<TileType>> tiles_;
};

} // namespace world
