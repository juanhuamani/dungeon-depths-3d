#include "world/Door.h"

namespace world {

Door::Door(DoorId id, RoomId roomA, RoomId roomB, CorridorId corridorId)
    : id_(id)
    , roomA_(roomA)
    , roomB_(roomB)
    , corridorId_(corridorId)
{
}

} // namespace world
