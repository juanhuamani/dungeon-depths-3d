#include "world/GridCell.h"

namespace world {

void GridCell::setEmpty() {
    state_ = CellState::Empty;
    roomId_ = kInvalidRoomId;
}

void GridCell::setRoom(RoomId roomId) {
    state_ = CellState::Room;
    roomId_ = roomId;
}

} // namespace world
