#pragma once

#include "world/DungeonTypes.h"

namespace world {

class Door {
public:
    Door() = default;
    Door(DoorId id, RoomId roomA, RoomId roomB, CorridorId corridorId);

    DoorId id() const { return id_; }
    RoomId roomA() const { return roomA_; }
    RoomId roomB() const { return roomB_; }
    CorridorId corridorId() const { return corridorId_; }
    bool isLocked() const { return locked_; }
    int keyId() const { return keyId_; }

    void setLocked(bool locked) { locked_ = locked; }
    void setKeyId(int keyId) { keyId_ = keyId; }

private:
    DoorId id_ = kInvalidDoorId;
    RoomId roomA_ = kInvalidRoomId;
    RoomId roomB_ = kInvalidRoomId;
    CorridorId corridorId_ = kInvalidCorridorId;
    bool locked_ = false;
    int keyId_ = -1;
};

} // namespace world
