#include "world/TileMap.h"

#include <array>
#include <cmath>
#include <queue>
#include <unordered_set>

namespace world {

namespace {

const std::array<TilePos, 4> kCardinalDirs = {{
    {-1, 0},
    {1, 0},
    {0, -1},
    {0, 1},
}};

} // namespace

bool isWalkable(TileType type) {
    switch (type) {
        case TileType::Room:
        case TileType::Corridor:
        case TileType::Door:
        case TileType::Entrance:
        case TileType::Exit:
            return true;
        default:
            return false;
    }
}

bool isRoomInterior(TileType type) {
    return type == TileType::Room
        || type == TileType::Entrance
        || type == TileType::Exit
        || type == TileType::Treasure;
}

bool isCarveable(TileType type) {
    return type == TileType::Empty || type == TileType::Wall;
}

TilePos directionOffset(Direction dir) {
    switch (dir) {
        case Direction::North: return {-1, 0};
        case Direction::South: return {1, 0};
        case Direction::East:  return {0, 1};
        case Direction::West:  return {0, -1};
    }
    return {};
}

Direction oppositeDirection(Direction dir) {
    switch (dir) {
        case Direction::North: return Direction::South;
        case Direction::South: return Direction::North;
        case Direction::East:  return Direction::West;
        case Direction::West:  return Direction::East;
    }
    return Direction::North;
}

TileMap::TileMap(int rows, int cols)
    : rows_(rows)
    , cols_(cols)
    , tiles_(static_cast<size_t>(rows), std::vector<TileType>(static_cast<size_t>(cols), TileType::Empty))
{
}

TileType TileMap::at(TilePos pos) const {
    if (!isInBounds(pos)) {
        return TileType::Empty;
    }
    return tiles_[static_cast<size_t>(pos.row)][static_cast<size_t>(pos.col)];
}

void TileMap::set(TilePos pos, TileType type) {
    if (!isInBounds(pos)) {
        return;
    }
    tiles_[static_cast<size_t>(pos.row)][static_cast<size_t>(pos.col)] = type;
}

bool TileMap::isInBounds(TilePos pos) const {
    return pos.row >= 0 && pos.row < rows_ && pos.col >= 0 && pos.col < cols_;
}

bool TileMap::canCarve(TilePos pos) const {
    if (!isInBounds(pos)) {
        return false;
    }
    return isCarveable(at(pos));
}

void TileMap::carveRoom(const TileRect& rect, TileType interiorType) {
    for (int r = rect.row; r < rect.row + rect.height; ++r) {
        for (int c = rect.col; c < rect.col + rect.width; ++c) {
            set({r, c}, interiorType);
        }
    }
    carveWallRing(rect);
}

void TileMap::carveWallRing(const TileRect& rect) {
    const int top = rect.row - 1;
    const int bottom = rect.row + rect.height;
    const int left = rect.col - 1;
    const int right = rect.col + rect.width;

    for (int c = left; c <= right; ++c) {
        if (canCarve({top, c}) || at({top, c}) == TileType::Empty) {
            set({top, c}, TileType::Wall);
        }
        if (canCarve({bottom, c}) || at({bottom, c}) == TileType::Empty) {
            set({bottom, c}, TileType::Wall);
        }
    }

    for (int r = top; r <= bottom; ++r) {
        if (canCarve({r, left}) || at({r, left}) == TileType::Empty) {
            set({r, left}, TileType::Wall);
        }
        if (canCarve({r, right}) || at({r, right}) == TileType::Empty) {
            set({r, right}, TileType::Wall);
        }
    }
}

bool TileMap::carveThickPoint(TilePos center, int width) {
    const int half = width / 2;
    for (int dr = -half; dr <= half; ++dr) {
        for (int dc = -half; dc <= half; ++dc) {
            TilePos pos{center.row + dr, center.col + dc};
            if (!isInBounds(pos)) {
                return false;
            }
            if (isRoomInterior(at(pos))) {
                return false;
            }
        }
    }

    for (int dr = -half; dr <= half; ++dr) {
        for (int dc = -half; dc <= half; ++dc) {
            TilePos pos{center.row + dr, center.col + dc};
            if (isInBounds(pos) && !isRoomInterior(at(pos))) {
                set(pos, TileType::Corridor);
            }
        }
    }
    return true;
}

std::vector<TilePos> TileMap::buildLPath(TilePos start, TilePos end, bool horizontalFirst) const {
    std::vector<TilePos> path;
    if (start == end) {
        path.push_back(start);
        return path;
    }

    TilePos corner;
    if (horizontalFirst) {
        corner = {start.row, end.col};
    } else {
        corner = {end.row, start.col};
    }

    auto appendSegment = [&path](TilePos from, TilePos to) {
        const int rowStep = (to.row > from.row) ? 1 : (to.row < from.row ? -1 : 0);
        const int colStep = (to.col > from.col) ? 1 : (to.col < from.col ? -1 : 0);
        TilePos current = from;
        while (current != to) {
            if (path.empty() || path.back() != current) {
                path.push_back(current);
            }
            current.row += rowStep;
            current.col += colStep;
        }
        if (path.empty() || path.back() != to) {
            path.push_back(to);
        }
    };

    appendSegment(start, corner);
    if (corner != end) {
        if (!path.empty() && path.back() == corner) {
            path.pop_back();
        }
        appendSegment(corner, end);
    }

    return path;
}

bool TileMap::carveCorridorPath(const std::vector<TilePos>& path, int width) {
    for (const TilePos& point : path) {
        if (!carveThickPoint(point, width)) {
            return false;
        }
    }
    return true;
}

bool TileMap::carveCorridorBetween(TilePos start, TilePos end, int width, bool horizontalFirst) {
    const std::vector<TilePos> path = buildLPath(start, end, horizontalFirst);
    return carveCorridorPath(path, width);
}

std::vector<TilePos> TileMap::getWalkableNeighbors(TilePos pos) const {
    std::vector<TilePos> neighbors;
    for (const TilePos& dir : kCardinalDirs) {
        TilePos next{pos.row + dir.row, pos.col + dir.col};
        if (isInBounds(next) && isWalkable(at(next))) {
            neighbors.push_back(next);
        }
    }
    return neighbors;
}

std::vector<TilePos> TileMap::findAllTilesOfType(TileType type) const {
    std::vector<TilePos> result;
    for (int r = 0; r < rows_; ++r) {
        for (int c = 0; c < cols_; ++c) {
            if (tiles_[static_cast<size_t>(r)][static_cast<size_t>(c)] == type) {
                result.push_back({r, c});
            }
        }
    }
    return result;
}

bool TileMap::isReachable(TilePos from, TilePos to) const {
    if (!isInBounds(from) || !isInBounds(to)) {
        return false;
    }
    if (!isWalkable(at(from)) || !isWalkable(at(to))) {
        return false;
    }

    std::queue<TilePos> frontier;
    std::unordered_set<int> visited;

    auto encode = [this](TilePos p) { return p.row * cols_ + p.col; };

    frontier.push(from);
    visited.insert(encode(from));

    while (!frontier.empty()) {
        TilePos current = frontier.front();
        frontier.pop();

        if (current == to) {
            return true;
        }

        for (TilePos neighbor : getWalkableNeighbors(current)) {
            if (visited.insert(encode(neighbor)).second) {
                frontier.push(neighbor);
            }
        }
    }

    return false;
}

bool TileMap::allRoomTilesReachable(TilePos from) const {
    if (!isInBounds(from) || !isWalkable(at(from))) {
        return false;
    }

    std::queue<TilePos> frontier;
    std::unordered_set<int> visited;
    int reachableInterior = 0;

    auto encode = [this](TilePos p) { return p.row * cols_ + p.col; };

    frontier.push(from);
    visited.insert(encode(from));

    while (!frontier.empty()) {
        TilePos current = frontier.front();
        frontier.pop();

        if (isRoomInterior(at(current))) {
            ++reachableInterior;
        }

        for (TilePos neighbor : getWalkableNeighbors(current)) {
            if (visited.insert(encode(neighbor)).second) {
                frontier.push(neighbor);
            }
        }
    }

    int totalInterior = 0;
    for (int r = 0; r < rows_; ++r) {
        for (int c = 0; c < cols_; ++c) {
            if (isRoomInterior(tiles_[static_cast<size_t>(r)][static_cast<size_t>(c)])) {
                ++totalInterior;
            }
        }
    }

    return reachableInterior == totalInterior;
}

} // namespace world
