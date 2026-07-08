#include "world/DungeonConsoleTest.h"

#include "world/DungeonGenerator.h"
#include "world/DungeonPrinter.h"

#include <iostream>

namespace world {

namespace {

bool validateDungeon(const Dungeon& dungeon) {
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
        if (room.connections().empty() || room.connections().size() > 4) {
            return false;
        }
    }

    return true;
}

} // namespace

void runDungeonConsoleTest() {
    constexpr int kDungeonCount = 5;
    int passed = 0;

    std::cout << "=== Prueba de generacion procedural de mazmorras ===\n\n";

    for (int i = 0; i < kDungeonCount; ++i) {
        GeneratorConfig config;
        config.rows = 12;
        config.cols = 16;
        config.mainPathMinLength = 6;
        config.mainPathMaxLength = 10;
        config.maxBranchLength = 3;
        config.seed = 0;

        DungeonGenerator generator;
        Dungeon dungeon = generator.generate(config);

        const bool ok = validateDungeon(dungeon);
        passed += ok ? 1 : 0;

        std::cout << "--- Dungeon #" << (i + 1) << (ok ? " [OK]" : " [FAIL]") << " ---\n";
        DungeonPrinter::printAll(std::cout, dungeon);
    }

    std::cout << "=== Resultado final: " << passed << "/" << kDungeonCount << " dungeons validos ===\n";

    if (passed == kDungeonCount) {
        std::cout << "Todas las pruebas pasaron.\n";
    } else {
        std::cout << "Algunas pruebas fallaron.\n";
    }
}

} // namespace world
