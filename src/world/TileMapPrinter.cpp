#include "world/TileMapPrinter.h"

#include <algorithm>

namespace world {

char TileMapPrinter::tileToChar(TileType type) {
    switch (type) {
        case TileType::Empty:    return ' ';
        case TileType::Wall:     return '#';
        case TileType::Room:     return '.';
        case TileType::Corridor: return '+';
        case TileType::Door:     return 'D';
        case TileType::Entrance: return 'E';
        case TileType::Exit:     return 'X';
        case TileType::Treasure: return 'T';
        case TileType::Enemy:    return 'M';
        case TileType::Key:      return 'K';
    }
    return '?';
}

void TileMapPrinter::printLegend(std::ostream& out) {
    out << "Leyenda: ' '=vacio  #=pared  .=sala  +=pasillo  D=puerta  E=entrada  X=salida\n";
}

std::optional<TileRect> TileMapPrinter::contentBounds(const TileMap& tileMap) {
    int minRow = tileMap.rows();
    int minCol = tileMap.cols();
    int maxRow = 0;
    int maxCol = 0;
    bool found = false;

    for (int r = 0; r < tileMap.rows(); ++r) {
        for (int c = 0; c < tileMap.cols(); ++c) {
            if (tileMap.at({r, c}) != TileType::Empty) {
                found = true;
                minRow = std::min(minRow, r);
                minCol = std::min(minCol, c);
                maxRow = std::max(maxRow, r);
                maxCol = std::max(maxCol, c);
            }
        }
    }

    if (!found) {
        return std::nullopt;
    }

    return TileRect{minRow, minCol, maxCol - minCol + 1, maxRow - minRow + 1};
}

void TileMapPrinter::print(std::ostream& out, const TileMap& tileMap) {
    printLegend(out);
    out << "Mapa fisico (" << tileMap.rows() << "x" << tileMap.cols() << "):\n";

    for (int r = 0; r < tileMap.rows(); ++r) {
        for (int c = 0; c < tileMap.cols(); ++c) {
            out << tileToChar(tileMap.at({r, c}));
        }
        out << '\n';
    }
    out << '\n';
}

void TileMapPrinter::printCompact(std::ostream& out, const TileMap& tileMap) {
    printLegend(out);

    const std::optional<TileRect> bounds = contentBounds(tileMap);
    if (!bounds.has_value()) {
        out << "Mapa fisico: vacio\n\n";
        return;
    }

    const TileRect& rect = bounds.value();
    out << "Mapa fisico compacto (" << rect.height << "x" << rect.width << "):\n";

    for (int r = rect.row; r < rect.row + rect.height; ++r) {
        for (int c = rect.col; c < rect.col + rect.width; ++c) {
            out << tileToChar(tileMap.at({r, c}));
        }
        out << '\n';
    }
    out << '\n';
}

} // namespace world
