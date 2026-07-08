#include "world/DungeonPrinter.h"

#include <iomanip>
#include <sstream>

namespace world {

void DungeonPrinter::printGrid(std::ostream& out, const Dungeon& dungeon) {
    if (dungeon.rows() == 0 || dungeon.cols() == 0) {
        out << "(matriz vacia)\n";
        return;
    }

    const int cellWidth = 3;

    auto printHorizontalBorder = [&]() {
        out << '+';
        for (int col = 0; col < dungeon.cols(); ++col) {
            out << std::string(static_cast<size_t>(cellWidth), '-');
            out << (col + 1 < dungeon.cols() ? '+' : '+');
        }
        out << '\n';
    };

    printHorizontalBorder();

    for (int row = 0; row < dungeon.rows(); ++row) {
        out << '|';
        for (int col = 0; col < dungeon.cols(); ++col) {
            std::optional<RoomId> roomId = dungeon.roomAt({row, col});
            if (!roomId.has_value()) {
                out << std::setw(cellWidth) << ' ';
            } else {
                out << ' ' << roomLabel(dungeon, roomId.value()) << ' ';
            }

            if (col + 1 < dungeon.cols()) {
                std::optional<RoomId> rightRoom = dungeon.roomAt({row, col + 1});
                if (roomId.has_value() && rightRoom.has_value() &&
                    hasHorizontalConnection(dungeon, roomId.value(), rightRoom.value())) {
                    out << ' ';
                } else {
                    out << '|';
                }
            }
        }
        out << "|\n";

        if (row + 1 < dungeon.rows()) {
            out << '+';
            for (int col = 0; col < dungeon.cols(); ++col) {
                std::optional<RoomId> topRoom = dungeon.roomAt({row, col});
                std::optional<RoomId> bottomRoom = dungeon.roomAt({row + 1, col});

                if (topRoom.has_value() && bottomRoom.has_value() &&
                    hasVerticalConnection(dungeon, topRoom.value(), bottomRoom.value())) {
                    out << std::string(static_cast<size_t>(cellWidth), ' ');
                } else {
                    out << std::string(static_cast<size_t>(cellWidth), '-');
                }

                out << '+';
            }
            out << '\n';
        }
    }

    printHorizontalBorder();
    out << "Leyenda: S=Entrada, E=Salida, digitos=habitaciones normales, espacio=conexion\n";
}

void DungeonPrinter::printGraph(std::ostream& out, const Dungeon& dungeon) {
    out << "=== Grafo de habitaciones ===\n";

    for (const Room& room : dungeon.rooms()) {
        out << "Room " << room.id()
            << " [" << roomTypeToString(room.type()) << "] "
            << "(" << room.gridPos().row << "," << room.gridPos().col << ") "
            << "conexiones=" << room.connections().size() << " -> [";

        for (size_t i = 0; i < room.connections().size(); ++i) {
            const Corridor* corridor = dungeon.getCorridor(room.connections()[i]);
            if (!corridor) {
                continue;
            }
            RoomId neighbor = corridor->otherRoom(room.id());
            out << neighbor;
            if (i + 1 < room.connections().size()) {
                out << ", ";
            }
        }

        out << "]\n";
    }

    out << "\n=== Corredores logicos ===\n";
    for (const Corridor& corridor : dungeon.corridors()) {
        out << "Corridor " << corridor.id()
            << ": " << corridor.roomA() << " <-> " << corridor.roomB() << '\n';
    }
}

void DungeonPrinter::printSummary(std::ostream& out, const Dungeon& dungeon) {
    std::vector<RoomId> path = dungeon.findPath(dungeon.entranceId(), dungeon.exitId());
    std::vector<RoomId> reachable = dungeon.getReachableRooms(dungeon.entranceId());

    out << "=== Resumen ===\n";
    out << "Tamano matriz: " << dungeon.rows() << "x" << dungeon.cols() << '\n';
    out << "Habitaciones: " << dungeon.rooms().size() << '\n';
    out << "Corredores: " << dungeon.corridors().size() << '\n';
    out << "Entrada: " << dungeon.entranceId() << '\n';
    out << "Salida: " << dungeon.exitId() << '\n';
    out << "Alcanzables: " << reachable.size() << "/" << dungeon.rooms().size() << '\n';
    out << "Mapa totalmente alcanzable: " << (dungeon.isFullyReachable() ? "si" : "no") << '\n';
    out << "Camino entrada->salida: ";

    if (path.empty()) {
        out << "(no existe)\n";
        return;
    }

    for (size_t i = 0; i < path.size(); ++i) {
        out << path[i];
        if (i + 1 < path.size()) {
            out << " -> ";
        }
    }
    out << '\n';
}

void DungeonPrinter::printAll(std::ostream& out, const Dungeon& dungeon) {
    printSummary(out, dungeon);
    out << '\n';
    printGrid(out, dungeon);
    out << '\n';
    printGraph(out, dungeon);
    out << '\n';
}

std::string DungeonPrinter::roomTypeToString(RoomType type) {
    switch (type) {
    case RoomType::Normal:
        return "Normal";
    case RoomType::Entrance:
        return "Entrance";
    case RoomType::Exit:
        return "Exit";
    case RoomType::Treasure:
        return "Treasure";
    case RoomType::Boss:
        return "Boss";
    default:
        return "Unknown";
    }
}

char DungeonPrinter::roomLabel(const Dungeon& dungeon, RoomId roomId) {
    const Room* room = dungeon.getRoom(roomId);
    if (!room) {
        return '?';
    }

    if (room->isEntrance()) {
        return 'S';
    }
    if (room->isExit()) {
        return 'E';
    }

    int label = roomId % 10;
    return static_cast<char>('0' + label);
}

bool DungeonPrinter::hasHorizontalConnection(const Dungeon& dungeon, RoomId left, RoomId right) {
    return dungeon.areConnected(left, right);
}

bool DungeonPrinter::hasVerticalConnection(const Dungeon& dungeon, RoomId top, RoomId bottom) {
    return dungeon.areConnected(top, bottom);
}

} // namespace world
