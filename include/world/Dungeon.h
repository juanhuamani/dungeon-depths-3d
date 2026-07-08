#pragma once

#include "world/Corridor.h"
#include "world/Door.h"
#include "world/DungeonTypes.h"
#include "world/GridCell.h"
#include "world/Room.h"

#include <optional>
#include <unordered_map>
#include <vector>

namespace world {

class Dungeon {
public:
    Dungeon() = default;
    explicit Dungeon(int rows, int cols);

    int rows() const { return rows_; }
    int cols() const { return cols_; }

    const std::vector<std::vector<GridCell>>& grid() const { return grid_; }
    const std::vector<Room>& rooms() const { return rooms_; }
    const std::vector<Corridor>& corridors() const { return corridors_; }
    const std::vector<Door>& doors() const { return doors_; }

    RoomId entranceId() const { return entranceId_; }
    RoomId exitId() const { return exitId_; }

    void setEntranceId(RoomId id) { entranceId_ = id; }
    void setExitId(RoomId id) { exitId_ = id; }

    void initGrid(int rows, int cols);
    void clear();

    bool isInBounds(GridPos pos) const;
    bool isCellFree(GridPos pos) const;
    std::optional<RoomId> roomAt(GridPos pos) const;

    std::vector<GridPos> getFreeNeighbors(GridPos pos) const;
    std::vector<RoomId> getNeighborRooms(RoomId roomId) const;
    std::vector<RoomId> getSpatiallyAdjacentRooms(RoomId roomId) const;

    Room& createRoom(GridPos pos, RoomType type = RoomType::Normal);
    std::optional<CorridorId> connectRooms(RoomId roomA, RoomId roomB);

    const Room* getRoom(RoomId roomId) const;
    Room* getRoom(RoomId roomId);
    const Corridor* getCorridor(CorridorId corridorId) const;

    bool areConnected(RoomId roomA, RoomId roomB) const;
    bool hasPath(RoomId from, RoomId to) const;
    bool isFullyReachable() const;

    std::vector<RoomId> findPath(RoomId from, RoomId to) const;
    std::vector<RoomId> getReachableRooms(RoomId from) const;

private:
    void rebuildRoomIndex();

    int rows_ = 0;
    int cols_ = 0;
    std::vector<std::vector<GridCell>> grid_;
    std::vector<Room> rooms_;
    std::vector<Corridor> corridors_;
    std::vector<Door> doors_;
    std::unordered_map<RoomId, size_t> roomIndex_;

    RoomId nextRoomId_ = 0;
    CorridorId nextCorridorId_ = 0;

    RoomId entranceId_ = kInvalidRoomId;
    RoomId exitId_ = kInvalidRoomId;
};

} // namespace world
