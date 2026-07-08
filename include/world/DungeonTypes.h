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

} // namespace world
