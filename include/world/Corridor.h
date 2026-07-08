#pragma once

#include "world/DungeonTypes.h"

namespace world {

class Corridor {
public:
    Corridor() = default;
    Corridor(CorridorId id, RoomId roomA, RoomId roomB);

    CorridorId id() const { return id_; }
    RoomId roomA() const { return roomA_; }
    RoomId roomB() const { return roomB_; }

    RoomId otherRoom(RoomId roomId) const;

private:
    CorridorId id_ = kInvalidCorridorId;
    RoomId roomA_ = kInvalidRoomId;
    RoomId roomB_ = kInvalidRoomId;
};

} // namespace world
