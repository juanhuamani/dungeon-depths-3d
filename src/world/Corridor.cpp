#include "world/Corridor.h"

namespace world {

Corridor::Corridor(CorridorId id, RoomId roomA, RoomId roomB)
    : id_(id)
    , roomA_(roomA)
    , roomB_(roomB)
{
}

RoomId Corridor::otherRoom(RoomId roomId) const {
    if (roomId == roomA_) {
        return roomB_;
    }
    if (roomId == roomB_) {
        return roomA_;
    }
    return kInvalidRoomId;
}

} // namespace world
