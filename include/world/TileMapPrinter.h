#pragma once

#include "world/TileMap.h"

#include <optional>
#include <ostream>

namespace world {

class TileMapPrinter {
public:
    static void print(std::ostream& out, const TileMap& tileMap);
    static void printCompact(std::ostream& out, const TileMap& tileMap);
    static void printLegend(std::ostream& out);
    static char tileToChar(TileType type);

private:
    static std::optional<TileRect> contentBounds(const TileMap& tileMap);
};

} // namespace world
