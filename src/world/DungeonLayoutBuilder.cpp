#include "world/DungeonLayoutBuilder.h"

#include <algorithm>
#include <random>
#include <unordered_map>

namespace world {

TileMap DungeonLayoutBuilder::build(Dungeon& dungeon, const LayoutConfig& config) {
    config_ = config;

    if (config_.seed == 0) {
        std::random_device device;
        rng_.seed(device());
    } else {
        rng_.seed(config_.seed);
    }

    for (int attempt = 0; attempt < config_.maxLayoutAttempts; ++attempt) {
        dungeon.clearPhysicalData();

        const auto [mapRows, mapCols] = calcMapDimensions(dungeon);
        TileMap tileMap(mapRows, mapCols);

        if (!placeRooms(dungeon, tileMap)) {
            continue;
        }
        if (!placeDoors(dungeon, tileMap)) {
            continue;
        }
        if (!carveCorridors(dungeon, tileMap)) {
            continue;
        }
        for (const Room& room : dungeon.rooms()) {
            for (const DoorPlacement& door : room.doorPlacements()) {
                tileMap.set(door.tilePos, TileType::Door);
            }
        }
        if (validateTileMap(dungeon, tileMap)) {
            return tileMap;
        }
    }

    dungeon.clearPhysicalData();
    const auto [mapRows, mapCols] = calcMapDimensions(dungeon);
    TileMap tileMap(mapRows, mapCols);
    placeRooms(dungeon, tileMap);
    placeDoors(dungeon, tileMap);
    carveCorridors(dungeon, tileMap);
    for (const Room& room : dungeon.rooms()) {
        for (const DoorPlacement& door : room.doorPlacements()) {
            tileMap.set(door.tilePos, TileType::Door);
        }
    }
    return tileMap;
}

std::pair<int, int> DungeonLayoutBuilder::calcMapDimensions(const Dungeon& dungeon) const {
    const int spacing = config_.cellSpacing();
    const int mapRows = dungeon.rows() * spacing + 2 * config_.mapMargin + config_.maxRoomSize;
    const int mapCols = dungeon.cols() * spacing + 2 * config_.mapMargin + config_.maxRoomSize;
    return {mapRows, mapCols};
}

TilePos DungeonLayoutBuilder::logicalToAnchor(GridPos gridPos) const {
    const int spacing = config_.cellSpacing();
    return {
        config_.mapMargin + gridPos.row * spacing,
        config_.mapMargin + gridPos.col * spacing
    };
}

TileType DungeonLayoutBuilder::roomTypeToTileType(RoomType type) const {
    switch (type) {
        case RoomType::Entrance: return TileType::Entrance;
        case RoomType::Exit:     return TileType::Exit;
        default:                 return TileType::Room;
    }
}

bool DungeonLayoutBuilder::placeRooms(Dungeon& dungeon, TileMap& tileMap) {
    for (Room& room : dungeon.rooms()) {
        const TilePos anchor = logicalToAnchor(room.gridPos());
        const int offsetRow = randomInt(0, config_.roomPadding);
        const int offsetCol = randomInt(0, config_.roomPadding);

        const int width = std::max(config_.minRoomSize, std::min(room.size().width, config_.maxRoomSize));
        const int height = std::max(config_.minRoomSize, std::min(room.size().height, config_.maxRoomSize));

        TileRect bounds{
            anchor.row + offsetRow,
            anchor.col + offsetCol,
            width,
            height
        };

        if (bounds.row < 1 || bounds.col < 1
            || bounds.southRow() >= tileMap.rows() - 1
            || bounds.eastCol() >= tileMap.cols() - 1) {
            return false;
        }

        room.setBounds(bounds);
        room.setPhysicalCenter(bounds.center());
        room.setSize(RoomSize{width, height});

        tileMap.carveRoom(bounds, roomTypeToTileType(room.type()));
    }
    return true;
}

std::optional<Direction> DungeonLayoutBuilder::getConnectionDirection(const Room& from, const Room& to) const {
    const int dRow = to.gridPos().row - from.gridPos().row;
    const int dCol = to.gridPos().col - from.gridPos().col;

    if (dRow == -1 && dCol == 0) return Direction::North;
    if (dRow == 1 && dCol == 0)  return Direction::South;
    if (dRow == 0 && dCol == 1)  return Direction::East;
    if (dRow == 0 && dCol == -1) return Direction::West;
    return std::nullopt;
}

std::optional<TilePos> DungeonLayoutBuilder::findDoorPosition(const TileRect& bounds, Direction dir, int usedOffset) const {
    const int offset = usedOffset * 2;

    switch (dir) {
        case Direction::North: {
            const int col = bounds.col + bounds.width / 2 + offset;
            if (col <= bounds.col || col >= bounds.eastCol()) return std::nullopt;
            return TilePos{bounds.row - 1, col};
        }
        case Direction::South: {
            const int col = bounds.col + bounds.width / 2 + offset;
            if (col <= bounds.col || col >= bounds.eastCol()) return std::nullopt;
            return TilePos{bounds.southRow(), col};
        }
        case Direction::West: {
            const int row = bounds.row + bounds.height / 2 + offset;
            if (row <= bounds.row || row >= bounds.southRow()) return std::nullopt;
            return TilePos{row, bounds.col - 1};
        }
        case Direction::East: {
            const int row = bounds.row + bounds.height / 2 + offset;
            if (row <= bounds.row || row >= bounds.southRow()) return std::nullopt;
            return TilePos{row, bounds.eastCol()};
        }
    }
    return std::nullopt;
}

TilePos DungeonLayoutBuilder::computeExitTile(TilePos doorPos, Direction dir) const {
    const TilePos off = directionOffset(dir);
    return {doorPos.row + off.row, doorPos.col + off.col};
}

bool DungeonLayoutBuilder::placeDoors(Dungeon& dungeon, TileMap& tileMap) {
    std::unordered_map<RoomId, std::unordered_map<int, int>> doorCountPerWall;

    for (Corridor& corridor : dungeon.corridors()) {
        Room* roomA = dungeon.getRoom(corridor.roomA());
        Room* roomB = dungeon.getRoom(corridor.roomB());
        if (!roomA || !roomB) {
            return false;
        }

        const auto dirA = getConnectionDirection(*roomA, *roomB);
        const auto dirB = getConnectionDirection(*roomB, *roomA);
        if (!dirA.has_value() || !dirB.has_value()) {
            return false;
        }

        const int wallKeyA = static_cast<int>(dirA.value());
        const int wallKeyB = static_cast<int>(dirB.value());
        const int offsetA = doorCountPerWall[roomA->id()][wallKeyA]++;
        const int offsetB = doorCountPerWall[roomB->id()][wallKeyB]++;

        const auto doorPosA = findDoorPosition(roomA->bounds(), dirA.value(), offsetA);
        const auto doorPosB = findDoorPosition(roomB->bounds(), dirB.value(), offsetB);
        if (!doorPosA.has_value() || !doorPosB.has_value()) {
            return false;
        }

        tileMap.set(doorPosA.value(), TileType::Door);
        tileMap.set(doorPosB.value(), TileType::Door);

        const TilePos exitA = computeExitTile(doorPosA.value(), dirA.value());
        const TilePos exitB = computeExitTile(doorPosB.value(), dirB.value());

        DoorPlacement placementA{doorPosA.value(), exitA, dirA.value(), corridor.id()};
        DoorPlacement placementB{doorPosB.value(), exitB, dirB.value(), corridor.id()};

        roomA->addDoorPlacement(placementA);
        roomB->addDoorPlacement(placementB);

        dungeon.addDoor(roomA->id(), roomB->id(), corridor.id(), doorPosA.value());
        dungeon.addDoor(roomB->id(), roomA->id(), corridor.id(), doorPosB.value());
    }

    return true;
}

bool DungeonLayoutBuilder::tryCarveCorridor(TileMap& tileMap, Corridor& corridor,
                                             TilePos start, TilePos end, int width) {
    const bool horizontalFirst = randomBool();

    auto attempt = [&](TilePos s, TilePos e, bool hFirst) -> bool {
        TileMap snapshot = tileMap;
        if (!snapshot.carveCorridorBetween(s, e, width, hFirst)) {
            return false;
        }
        corridor.setCarvedPath(snapshot.buildLPath(s, e, hFirst));
        tileMap = std::move(snapshot);
        return true;
    };

    if (attempt(start, end, horizontalFirst)) {
        return true;
    }
    if (attempt(start, end, !horizontalFirst)) {
        return true;
    }

    for (int shift = 1; shift <= 2; ++shift) {
        for (int sign : {-1, 1}) {
            const TilePos s1{start.row + sign * shift, start.col};
            const TilePos e1{end.row + sign * shift, end.col};
            if (attempt(s1, e1, horizontalFirst) || attempt(s1, e1, !horizontalFirst)) {
                return true;
            }

            const TilePos s2{start.row, start.col + sign * shift};
            const TilePos e2{end.row, end.col + sign * shift};
            if (attempt(s2, e2, horizontalFirst) || attempt(s2, e2, !horizontalFirst)) {
                return true;
            }
        }
    }

    return false;
}

bool DungeonLayoutBuilder::carveCorridors(Dungeon& dungeon, TileMap& tileMap) {
    for (Corridor& corridor : dungeon.corridors()) {
        Room* roomA = dungeon.getRoom(corridor.roomA());
        Room* roomB = dungeon.getRoom(corridor.roomB());
        if (!roomA || !roomB) {
            return false;
        }

        TilePos start{};
        TilePos end{};
        bool foundStart = false;
        bool foundEnd = false;

        for (const DoorPlacement& dp : roomA->doorPlacements()) {
            if (dp.corridorId == corridor.id()) {
                start = dp.exitTile;
                foundStart = true;
                break;
            }
        }
        for (const DoorPlacement& dp : roomB->doorPlacements()) {
            if (dp.corridorId == corridor.id()) {
                end = dp.exitTile;
                foundEnd = true;
                break;
            }
        }

        if (!foundStart || !foundEnd) {
            return false;
        }

        if (!tryCarveCorridor(tileMap, corridor, start, end, config_.corridorWidth)) {
            return false;
        }
    }
    return true;
}

bool DungeonLayoutBuilder::validateTileMap(const Dungeon& dungeon, const TileMap& tileMap) const {
    const Room* entrance = dungeon.getRoom(dungeon.entranceId());
    const Room* exitRoom = dungeon.getRoom(dungeon.exitId());
    if (!entrance || !exitRoom) {
        return false;
    }

    const TilePos entranceCenter = entrance->physicalCenter();
    const TilePos exitCenter = exitRoom->physicalCenter();

    if (!tileMap.isReachable(entranceCenter, exitCenter)) {
        return false;
    }

    if (!tileMap.allRoomTilesReachable(entranceCenter)) {
        return false;
    }

    return true;
}

int DungeonLayoutBuilder::randomInt(int min, int max) {
    if (min >= max) {
        return min;
    }
    std::uniform_int_distribution<int> dist(min, max);
    return dist(rng_);
}

bool DungeonLayoutBuilder::randomBool() {
    std::uniform_int_distribution<int> dist(0, 1);
    return dist(rng_) == 1;
}

} // namespace world
