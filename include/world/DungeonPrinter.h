#pragma once

#include "world/Dungeon.h"

#include <ostream>
#include <string>

namespace world {

class DungeonPrinter {
public:
    static void printGrid(std::ostream& out, const Dungeon& dungeon);
    static void printGraph(std::ostream& out, const Dungeon& dungeon);
    static void printSummary(std::ostream& out, const Dungeon& dungeon);
    static void printAll(std::ostream& out, const Dungeon& dungeon);

private:
    static std::string roomTypeToString(RoomType type);
    static char roomLabel(const Dungeon& dungeon, RoomId roomId);
    static bool hasHorizontalConnection(const Dungeon& dungeon, RoomId left, RoomId right);
    static bool hasVerticalConnection(const Dungeon& dungeon, RoomId top, RoomId bottom);
};

} // namespace world
