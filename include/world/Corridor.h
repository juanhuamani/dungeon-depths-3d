#pragma once

#include "world/DungeonTypes.h"

#include <vector>

namespace world {

class Corridor {
public:
    Corridor() = default;
    Corridor(CorridorId id, RoomId roomA, RoomId roomB);

    CorridorId id() const { return id_; }
    RoomId roomA() const { return roomA_; }
    RoomId roomB() const { return roomB_; }

    RoomId otherRoom(RoomId roomId) const;

    const std::vector<TilePos>& carvedPath() const { return carvedPath_; }
    void setCarvedPath(std::vector<TilePos> path) { carvedPath_ = std::move(path); }
    void appendToCarvedPath(const std::vector<TilePos>& segment);
    void clearPhysicalData() { carvedPath_.clear(); }

private:
    CorridorId id_ = kInvalidCorridorId;
    RoomId roomA_ = kInvalidRoomId;
    RoomId roomB_ = kInvalidRoomId;
    std::vector<TilePos> carvedPath_{};
};

} // namespace world
