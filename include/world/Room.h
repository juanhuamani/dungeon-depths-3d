#pragma once

#include "world/DungeonTypes.h"

#include <vector>

namespace world {

class Room {
public:
    Room() = default;
    Room(RoomId id, GridPos gridPos, RoomType type);

    RoomId id() const { return id_; }
    GridPos gridPos() const { return gridPos_; }
    RoomType type() const { return type_; }
    RoomSize size() const { return size_; }
    GridPos center() const { return center_; }
    const std::vector<CorridorId>& connections() const { return connections_; }

    void setType(RoomType type) { type_ = type; }
    void setSize(RoomSize size) { size_ = size; }
    void setCenter(GridPos center) { center_ = center; }

    bool canAddConnection() const;
    bool hasConnection(CorridorId corridorId) const;
    void addConnection(CorridorId corridorId);

    bool isEntrance() const { return type_ == RoomType::Entrance; }
    bool isExit() const { return type_ == RoomType::Exit; }

private:
    RoomId id_ = kInvalidRoomId;
    GridPos gridPos_{};
    std::vector<CorridorId> connections_;
    RoomType type_ = RoomType::Normal;
    RoomSize size_{};
    GridPos center_{};
};

} // namespace world
