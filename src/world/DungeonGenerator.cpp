#include "world/DungeonGenerator.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <random>
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

Dungeon DungeonGenerator::generate(const GeneratorConfig& config) {
    config_ = config;
    mainPath_.clear();

    if (config_.seed == 0) {
        std::random_device device;
        rng_.seed(device());
    } else {
        rng_.seed(config_.seed);
    }

    for (int attempt = 0; attempt < config_.maxGenerationAttempts; ++attempt) {
        Dungeon dungeon;
        stage1_initGrid(dungeon);

        if (!stage2_placeEntrance(dungeon)) {
            continue;
        }

        if (!stage3_buildMainPath(dungeon)) {
            continue;
        }

        stage4_addBranches(dungeon);
        stage5_addExtraConnections(dungeon);

        if (stage6_validate(dungeon)) {
            return dungeon;
        }
    }

    Dungeon fallback;
    stage1_initGrid(fallback);
    stage2_placeEntrance(fallback);
    stage3_buildMainPath(fallback);
    stage4_addBranches(fallback);
    stage5_addExtraConnections(fallback);
    return fallback;
}

void DungeonGenerator::stage1_initGrid(Dungeon& dungeon) {
    dungeon.initGrid(config_.rows, config_.cols);
    mainPath_.clear();
}

bool DungeonGenerator::stage2_placeEntrance(Dungeon& dungeon) {
    std::optional<GridPos> entrancePos = pickRandomBorderCell(dungeon);
    if (!entrancePos.has_value()) {
        return false;
    }

    Room& entrance = createRoomWithRandomSize(dungeon, entrancePos.value(), RoomType::Entrance);
    dungeon.setEntranceId(entrance.id());
    mainPath_.push_back(entrance.id());
    return true;
}

bool DungeonGenerator::stage3_buildMainPath(Dungeon& dungeon) {
    if (mainPath_.empty()) {
        return false;
    }

    const Room* entrance = dungeon.getRoom(mainPath_.front());
    if (!entrance) {
        return false;
    }

    std::optional<GridPos> targetPos = pickOppositeBorderTarget(dungeon, entrance->gridPos());
    if (!targetPos.has_value()) {
        return false;
    }

    const int targetLength = randomInt(config_.mainPathMinLength, config_.mainPathMaxLength);
    RoomId currentRoomId = mainPath_.front();
    std::vector<RoomId> pathStack{currentRoomId};

    while (static_cast<int>(mainPath_.size()) < targetLength) {
        const Room* currentRoom = dungeon.getRoom(currentRoomId);
        if (!currentRoom) {
            break;
        }

        std::vector<GridPos> freeNeighbors = dungeon.getFreeNeighbors(currentRoom->gridPos());
        if (!freeNeighbors.empty()) {
            std::vector<GridPos> scored;
            scored.reserve(freeNeighbors.size());

            for (const GridPos& pos : freeNeighbors) {
                if (manhattanDistance(pos, targetPos.value()) <=
                    manhattanDistance(currentRoom->gridPos(), targetPos.value()) ||
                    chance(0.35f)) {
                    scored.push_back(pos);
                }
            }

            if (scored.empty()) {
                scored = freeNeighbors;
            }

            GridPos nextPos = randomChoice(scored);
            Room& nextRoom = createRoomWithRandomSize(dungeon, nextPos, RoomType::Normal);
            tryConnectAdjacentRooms(dungeon, currentRoomId, nextRoom.id());

            currentRoomId = nextRoom.id();
            mainPath_.push_back(currentRoomId);
            pathStack.push_back(currentRoomId);
            continue;
        }

        if (pathStack.size() <= 1) {
            break;
        }

        pathStack.pop_back();
        currentRoomId = pathStack.back();
    }

    const Room* currentRoom = dungeon.getRoom(currentRoomId);
    if (!currentRoom) {
        return false;
    }

    std::vector<GridPos> exitCandidates = dungeon.getFreeNeighbors(currentRoom->gridPos());
    exitCandidates.erase(
        std::remove_if(exitCandidates.begin(), exitCandidates.end(),
                       [&](const GridPos& pos) { return !isOnBorder(dungeon, pos); }),
        exitCandidates.end());

    if (exitCandidates.empty()) {
        exitCandidates = dungeon.getFreeNeighbors(currentRoom->gridPos());
    }

    if (exitCandidates.empty()) {
        Room* lastRoom = dungeon.getRoom(mainPath_.back());
        if (!lastRoom) {
            return false;
        }
        lastRoom->setType(RoomType::Exit);
        dungeon.setExitId(lastRoom->id());
        return mainPath_.size() >= 2;
    }

    GridPos exitPos = randomChoice(exitCandidates);
    Room& exitRoom = createRoomWithRandomSize(dungeon, exitPos, RoomType::Exit);
    tryConnectAdjacentRooms(dungeon, currentRoomId, exitRoom.id());

    mainPath_.push_back(exitRoom.id());
    dungeon.setExitId(exitRoom.id());
    return true;
}

void DungeonGenerator::stage4_addBranches(Dungeon& dungeon) {
    std::vector<RoomId> branchOrigins = mainPath_;

    for (RoomId originId : branchOrigins) {
        const Room* origin = dungeon.getRoom(originId);
        if (!origin) {
            continue;
        }

        float probability = config_.branchProbability;
        if (origin->isEntrance() || origin->isExit()) {
            probability *= 0.5f;
        }

        if (!chance(probability)) {
            continue;
        }

        int branchCount = chance(0.5f) ? 2 : 1;
        for (int branch = 0; branch < branchCount; ++branch) {
            RoomId branchTip = originId;
            int branchLength = randomInt(1, config_.maxBranchLength);

            for (int step = 0; step < branchLength; ++step) {
                const Room* tipRoom = dungeon.getRoom(branchTip);
                if (!tipRoom) {
                    break;
                }

                std::vector<GridPos> freeNeighbors = dungeon.getFreeNeighbors(tipRoom->gridPos());
                freeNeighbors.erase(
                    std::remove_if(freeNeighbors.begin(), freeNeighbors.end(),
                                   [&](const GridPos& pos) {
                                       std::optional<RoomId> existing = dungeon.roomAt(pos);
                                       if (!existing.has_value()) {
                                           return false;
                                       }
                                       return std::find(mainPath_.begin(), mainPath_.end(),
                                                        existing.value()) != mainPath_.end();
                                   }),
                    freeNeighbors.end());

                if (freeNeighbors.empty()) {
                    break;
                }

                GridPos nextPos = randomChoice(freeNeighbors);
                Room& branchRoom = createRoomWithRandomSize(dungeon, nextPos, RoomType::Normal);
                tryConnectAdjacentRooms(dungeon, branchTip, branchRoom.id());
                branchTip = branchRoom.id();
            }
        }
    }
}

void DungeonGenerator::stage5_addExtraConnections(Dungeon& dungeon) {
    std::vector<std::pair<RoomId, RoomId>> candidates;

    for (const Room& room : dungeon.rooms()) {
        for (RoomId adjacentId : dungeon.getSpatiallyAdjacentRooms(room.id())) {
            if (room.id() >= adjacentId) {
                continue;
            }
            if (dungeon.areConnected(room.id(), adjacentId)) {
                continue;
            }

            const Room* adjacentRoom = dungeon.getRoom(adjacentId);
            if (!adjacentRoom) {
                continue;
            }
            if (!room.canAddConnection() || !adjacentRoom->canAddConnection()) {
                continue;
            }

            candidates.emplace_back(room.id(), adjacentId);
        }
    }

    std::shuffle(candidates.begin(), candidates.end(), rng_);

    for (const auto& [roomA, roomB] : candidates) {
        if (!chance(config_.extraConnectionProbability)) {
            continue;
        }
        dungeon.connectRooms(roomA, roomB);
    }
}

bool DungeonGenerator::stage6_validate(const Dungeon& dungeon) const {
    if (dungeon.entranceId() == kInvalidRoomId || dungeon.exitId() == kInvalidRoomId) {
        return false;
    }

    if (!dungeon.hasPath(dungeon.entranceId(), dungeon.exitId())) {
        return false;
    }

    if (!dungeon.isFullyReachable()) {
        return false;
    }

    for (const Room& room : dungeon.rooms()) {
        if (room.connections().empty()) {
            return false;
        }
        if (room.connections().size() > 4) {
            return false;
        }
    }

    return true;
}

std::optional<GridPos> DungeonGenerator::pickRandomBorderCell(const Dungeon& dungeon) const {
    std::vector<GridPos> borderCells;

    for (int row = 0; row < dungeon.rows(); ++row) {
        for (int col = 0; col < dungeon.cols(); ++col) {
            GridPos pos{row, col};
            if (isOnBorder(dungeon, pos) && dungeon.isCellFree(pos)) {
                borderCells.push_back(pos);
            }
        }
    }

    if (borderCells.empty()) {
        return std::nullopt;
    }

    return randomChoice(borderCells);
}

std::optional<GridPos> DungeonGenerator::pickRandomFreeNeighbor(const Dungeon& dungeon, GridPos pos) const {
    std::vector<GridPos> neighbors = dungeon.getFreeNeighbors(pos);
    if (neighbors.empty()) {
        return std::nullopt;
    }
    return randomChoice(neighbors);
}

std::optional<GridPos> DungeonGenerator::pickOppositeBorderTarget(const Dungeon& dungeon, GridPos entrancePos) const {
    std::vector<GridPos> borderCells;

    const bool entranceOnTop = entrancePos.row == 0;
    const bool entranceOnBottom = entrancePos.row == dungeon.rows() - 1;
    const bool entranceOnLeft = entrancePos.col == 0;
    const bool entranceOnRight = entrancePos.col == dungeon.cols() - 1;

    for (int row = 0; row < dungeon.rows(); ++row) {
        for (int col = 0; col < dungeon.cols(); ++col) {
            GridPos pos{row, col};
            if (!isOnBorder(dungeon, pos)) {
                continue;
            }

            if ((entranceOnTop && pos.row == dungeon.rows() - 1) ||
                (entranceOnBottom && pos.row == 0) ||
                (entranceOnLeft && pos.col == dungeon.cols() - 1) ||
                (entranceOnRight && pos.col == 0)) {
                borderCells.push_back(pos);
            }
        }
    }

    if (borderCells.empty()) {
        for (int row = 0; row < dungeon.rows(); ++row) {
            for (int col = 0; col < dungeon.cols(); ++col) {
                GridPos pos{row, col};
                if (isOnBorder(dungeon, pos)) {
                    borderCells.push_back(pos);
                }
            }
        }
    }

    if (borderCells.empty()) {
        return std::nullopt;
    }

    return randomChoice(borderCells);
}

bool DungeonGenerator::isOnBorder(const Dungeon& dungeon, GridPos pos) const {
    return pos.row == 0 || pos.col == 0 ||
           pos.row == dungeon.rows() - 1 || pos.col == dungeon.cols() - 1;
}

int DungeonGenerator::manhattanDistance(GridPos a, GridPos b) const {
    return std::abs(a.row - b.row) + std::abs(a.col - b.col);
}

Room& DungeonGenerator::createRoomWithRandomSize(Dungeon& dungeon, GridPos pos, RoomType type) {
    Room& room = dungeon.createRoom(pos, type);
    room.setSize(RoomSize{randomInt(3, 6), randomInt(3, 6)});
    room.setCenter(pos);
    return room;
}

bool DungeonGenerator::tryConnectAdjacentRooms(Dungeon& dungeon, RoomId roomA, RoomId roomB) {
    const Room* first = dungeon.getRoom(roomA);
    const Room* second = dungeon.getRoom(roomB);
    if (!first || !second) {
        return false;
    }

    const int rowDiff = std::abs(first->gridPos().row - second->gridPos().row);
    const int colDiff = std::abs(first->gridPos().col - second->gridPos().col);
    if (rowDiff + colDiff != 1) {
        return false;
    }

    return dungeon.connectRooms(roomA, roomB).has_value();
}

bool DungeonGenerator::chance(float probability) {
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    return dist(rng_) < probability;
}

int DungeonGenerator::randomInt(int min, int max) {
    std::uniform_int_distribution<int> dist(min, max);
    return dist(rng_);
}

GridPos DungeonGenerator::randomChoice(const std::vector<GridPos>& options) const {
    std::uniform_int_distribution<size_t> dist(0, options.size() - 1);
    return options[dist(rng_)];
}

} // namespace world
