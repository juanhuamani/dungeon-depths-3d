#include "world/Dungeon.h"

#include <algorithm>
#include <array>
#include <queue>
#include <unordered_set>

namespace world {

namespace {

const std::array<GridPos, 4> kDirections = {{
    {-1, 0},
    {1, 0},
    {0, -1},
    {0, 1},
}};

} // namespace

Dungeon::Dungeon(int rows, int cols) {
    initGrid(rows, cols);
}

void Dungeon::initGrid(int rows, int cols) {
    clear();
    rows_ = rows;
    cols_ = cols;
    grid_.assign(static_cast<size_t>(rows), std::vector<GridCell>(static_cast<size_t>(cols)));
}

void Dungeon::clear() {
    rows_ = 0;
    cols_ = 0;
    grid_.clear();
    rooms_.clear();
    corridors_.clear();
    doors_.clear();
    roomIndex_.clear();
    nextRoomId_ = 0;
    nextCorridorId_ = 0;
    nextDoorId_ = 0;
    entranceId_ = kInvalidRoomId;
    exitId_ = kInvalidRoomId;
}

void Dungeon::clearPhysicalData() {
    for (Room& room : rooms_) {
        room.clearPhysicalData();
    }
    for (Corridor& corridor : corridors_) {
        corridor.clearPhysicalData();
    }
    doors_.clear();
    nextDoorId_ = 0;
}

bool Dungeon::isInBounds(GridPos pos) const {
    return pos.row >= 0 && pos.row < rows_ && pos.col >= 0 && pos.col < cols_;
}

bool Dungeon::isCellFree(GridPos pos) const {
    if (!isInBounds(pos)) {
        return false;
    }
    return grid_[static_cast<size_t>(pos.row)][static_cast<size_t>(pos.col)].isEmpty();
}

std::optional<RoomId> Dungeon::roomAt(GridPos pos) const {
    if (!isInBounds(pos)) {
        return std::nullopt;
    }

    const GridCell& cell = grid_[static_cast<size_t>(pos.row)][static_cast<size_t>(pos.col)];
    if (cell.isEmpty()) {
        return std::nullopt;
    }
    return cell.roomId();
}

std::vector<GridPos> Dungeon::getFreeNeighbors(GridPos pos) const {
    std::vector<GridPos> neighbors;
    neighbors.reserve(4);

    for (const GridPos& dir : kDirections) {
        GridPos neighbor{pos.row + dir.row, pos.col + dir.col};
        if (isCellFree(neighbor)) {
            neighbors.push_back(neighbor);
        }
    }

    return neighbors;
}

std::vector<RoomId> Dungeon::getNeighborRooms(RoomId roomId) const {
    std::vector<RoomId> neighbors;
    const Room* room = getRoom(roomId);
    if (!room) {
        return neighbors;
    }

    for (CorridorId corridorId : room->connections()) {
        const Corridor* corridor = getCorridor(corridorId);
        if (!corridor) {
            continue;
        }
        RoomId other = corridor->otherRoom(roomId);
        if (other != kInvalidRoomId) {
            neighbors.push_back(other);
        }
    }

    return neighbors;
}

std::vector<RoomId> Dungeon::getSpatiallyAdjacentRooms(RoomId roomId) const {
    std::vector<RoomId> adjacent;
    const Room* room = getRoom(roomId);
    if (!room) {
        return adjacent;
    }

    for (const GridPos& dir : kDirections) {
        GridPos neighborPos{room->gridPos().row + dir.row, room->gridPos().col + dir.col};
        std::optional<RoomId> neighborRoomId = roomAt(neighborPos);
        if (neighborRoomId.has_value() && neighborRoomId.value() != roomId) {
            adjacent.push_back(neighborRoomId.value());
        }
    }

    return adjacent;
}

Room& Dungeon::createRoom(GridPos pos, RoomType type) {
    Room room(nextRoomId_, pos, type);
    room.setCenter(pos);

    RoomId roomId = nextRoomId_++;
    rooms_.push_back(std::move(room));
    roomIndex_[roomId] = rooms_.size() - 1;

    grid_[static_cast<size_t>(pos.row)][static_cast<size_t>(pos.col)].setRoom(roomId);
    return rooms_.back();
}

std::optional<CorridorId> Dungeon::connectRooms(RoomId roomA, RoomId roomB) {
    if (roomA == roomB || areConnected(roomA, roomB)) {
        return std::nullopt;
    }

    Room* first = getRoom(roomA);
    Room* second = getRoom(roomB);
    if (!first || !second) {
        return std::nullopt;
    }

    if (!first->canAddConnection() || !second->canAddConnection()) {
        return std::nullopt;
    }

    CorridorId corridorId = nextCorridorId_++;
    corridors_.emplace_back(corridorId, roomA, roomB);
    first->addConnection(corridorId);
    second->addConnection(corridorId);

    return corridorId;
}

const Room* Dungeon::getRoom(RoomId roomId) const {
    auto it = roomIndex_.find(roomId);
    if (it == roomIndex_.end()) {
        return nullptr;
    }
    return &rooms_[it->second];
}

Room* Dungeon::getRoom(RoomId roomId) {
    auto it = roomIndex_.find(roomId);
    if (it == roomIndex_.end()) {
        return nullptr;
    }
    return &rooms_[it->second];
}

const Corridor* Dungeon::getCorridor(CorridorId corridorId) const {
    if (corridorId < 0 || corridorId >= static_cast<CorridorId>(corridors_.size())) {
        return nullptr;
    }
    return &corridors_[static_cast<size_t>(corridorId)];
}

Corridor* Dungeon::getCorridor(CorridorId corridorId) {
    if (corridorId < 0 || corridorId >= static_cast<CorridorId>(corridors_.size())) {
        return nullptr;
    }
    return &corridors_[static_cast<size_t>(corridorId)];
}

Door& Dungeon::addDoor(RoomId ownerRoomId, RoomId otherRoomId, CorridorId corridorId, TilePos tilePos) {
    DoorId doorId = nextDoorId_++;
    Door door(doorId, ownerRoomId, otherRoomId, corridorId);
    door.setOwnerRoomId(ownerRoomId);
    door.setTilePos(tilePos);
    doors_.push_back(std::move(door));
    return doors_.back();
}

bool Dungeon::areConnected(RoomId roomA, RoomId roomB) const {
    const Room* room = getRoom(roomA);
    if (!room) {
        return false;
    }

    for (CorridorId corridorId : room->connections()) {
        const Corridor* corridor = getCorridor(corridorId);
        if (!corridor) {
            continue;
        }
        if (corridor->roomA() == roomB || corridor->roomB() == roomB) {
            return true;
        }
    }

    return false;
}

bool Dungeon::hasPath(RoomId from, RoomId to) const {
    return !findPath(from, to).empty();
}

bool Dungeon::isFullyReachable() const {
    if (entranceId_ == kInvalidRoomId || rooms_.empty()) {
        return false;
    }

    std::vector<RoomId> reachable = getReachableRooms(entranceId_);
    return reachable.size() == rooms_.size();
}

std::vector<RoomId> Dungeon::findPath(RoomId from, RoomId to) const {
    if (from == kInvalidRoomId || to == kInvalidRoomId || from == to) {
        return from == to ? std::vector<RoomId>{from} : std::vector<RoomId>{};
    }

    if (!getRoom(from) || !getRoom(to)) {
        return {};
    }

    std::queue<RoomId> frontier;
    std::unordered_map<RoomId, RoomId> cameFrom;

    frontier.push(from);
    cameFrom[from] = from;

    while (!frontier.empty()) {
        RoomId current = frontier.front();
        frontier.pop();

        if (current == to) {
            break;
        }

        for (RoomId neighbor : getNeighborRooms(current)) {
            if (cameFrom.find(neighbor) != cameFrom.end()) {
                continue;
            }
            cameFrom[neighbor] = current;
            frontier.push(neighbor);
        }
    }

    if (cameFrom.find(to) == cameFrom.end()) {
        return {};
    }

    std::vector<RoomId> path;
    RoomId current = to;
    while (current != from) {
        path.push_back(current);
        current = cameFrom[current];
    }
    path.push_back(from);
    std::reverse(path.begin(), path.end());
    return path;
}

std::vector<RoomId> Dungeon::getReachableRooms(RoomId from) const {
    std::vector<RoomId> reachable;
    if (!getRoom(from)) {
        return reachable;
    }

    std::queue<RoomId> frontier;
    std::unordered_set<RoomId> visited;

    frontier.push(from);
    visited.insert(from);

    while (!frontier.empty()) {
        RoomId current = frontier.front();
        frontier.pop();
        reachable.push_back(current);

        for (RoomId neighbor : getNeighborRooms(current)) {
            if (visited.insert(neighbor).second) {
                frontier.push(neighbor);
            }
        }
    }

    return reachable;
}

void Dungeon::rebuildRoomIndex() {
    roomIndex_.clear();
    for (size_t i = 0; i < rooms_.size(); ++i) {
        roomIndex_[rooms_[i].id()] = i;
    }
}

} // namespace world
