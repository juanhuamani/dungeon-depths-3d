#include "world/Room.h"

#include <algorithm>

namespace world {

Room::Room(RoomId id, GridPos gridPos, RoomType type)
    : id_(id)
    , gridPos_(gridPos)
    , type_(type)
    , center_(gridPos)
{
}

bool Room::canAddConnection() const {
    return connections_.size() < 4;
}

bool Room::hasConnection(CorridorId corridorId) const {
    return std::find(connections_.begin(), connections_.end(), corridorId) != connections_.end();
}

void Room::addConnection(CorridorId corridorId) {
    if (!canAddConnection() || hasConnection(corridorId)) {
        return;
    }
    connections_.push_back(corridorId);
}

} // namespace world
