#include "world/TileMapPrinter.h"

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

    const std::optional<TileRect> bounds = tileMap.contentBounds();
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
