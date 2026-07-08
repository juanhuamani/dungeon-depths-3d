#pragma once

#include "world/DungeonTypes.h"

namespace world {

class GridCell {
public:
    GridCell() = default;

    CellState state() const { return state_; }
    RoomId roomId() const { return roomId_; }

    bool isEmpty() const { return state_ == CellState::Empty; }
    bool hasRoom() const { return state_ == CellState::Room; }

    void setEmpty();
    void setRoom(RoomId roomId);

private:
    CellState state_ = CellState::Empty;
    RoomId roomId_ = kInvalidRoomId;
};

} // namespace world
