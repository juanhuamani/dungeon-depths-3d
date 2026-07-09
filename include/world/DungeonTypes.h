#pragma once

#include <cstdint>

namespace world {

using RoomId = int;
using CorridorId = int;
using DoorId = int;

constexpr RoomId kInvalidRoomId = -1;
constexpr CorridorId kInvalidCorridorId = -1;
constexpr DoorId kInvalidDoorId = -1;

enum class CellState {
    Empty,
    Room
};

enum class RoomType {
    Normal,
    Entrance,
    Exit,
    Treasure,
    Boss
};

struct GridPos {
    int row = 0;
    int col = 0;

    bool operator==(const GridPos& other) const {
        return row == other.row && col == other.col;
    }

    bool operator!=(const GridPos& other) const {
        return !(*this == other);
    }
};

struct RoomSize {
    int width = 4;
    int height = 4;
};

struct GeneratorConfig {
    int rows = 20;
    int cols = 20;
    int mainPathMinLength = 8;
    int mainPathMaxLength = 15;
    int maxBranchLength = 4;
    float branchProbability = 0.6f;
    float extraConnectionProbability = 0.15f;
    uint32_t seed = 0;
    int maxGenerationAttempts = 10;
};

enum class Direction {
    North,
    South,
    East,
    West
};

enum class TileType {
    Empty,
    Room,
    Corridor,
    Door,
    Wall,
    Entrance,
    Exit,
    Treasure,
    Enemy,
    Key
};

struct TilePos {
    int row = 0;
    int col = 0;

    bool operator==(const TilePos& other) const {
        return row == other.row && col == other.col;
    }

    bool operator!=(const TilePos& other) const {
        return !(*this == other);
    }

    TilePos operator+(const TilePos& other) const {
        return {row + other.row, col + other.col};
    }
};

struct TileRect {
    int row = 0;
    int col = 0;
    int width = 0;
    int height = 0;

    int southRow() const { return row + height; }
    int eastCol() const { return col + width; }
    TilePos center() const { return {row + height / 2, col + width / 2}; }
};

struct DoorPlacement {
    TilePos tilePos{};
    TilePos exitTile{};
    Direction direction = Direction::North;
    CorridorId corridorId = kInvalidCorridorId;
};

struct LayoutConfig {
    int minRoomSize = 3;
    int maxRoomSize = 6;
    int corridorWidth = 3;
    int wallThickness = 1;
    int roomPadding = 4;
    int mapMargin = 8;
    int maxLayoutAttempts = 5;
    uint32_t seed = 0;

    int cellSpacing() const {
        return maxRoomSize + corridorWidth + 2 * wallThickness + roomPadding;
    }
};

bool isWalkable(TileType type);
bool isRoomInterior(TileType type);
bool isCarveable(TileType type);

TilePos directionOffset(Direction dir);
Direction oppositeDirection(Direction dir);

} // namespace world
