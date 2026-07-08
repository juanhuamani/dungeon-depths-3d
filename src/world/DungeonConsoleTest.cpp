#include "world/DungeonConsoleTest.h"

#include "world/DungeonGenerator.h"
#include "world/DungeonLayoutBuilder.h"
#include "world/DungeonPrinter.h"
#include "world/TileMapPrinter.h"

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

bool validatePhysicalLayout(const Dungeon& dungeon, const TileMap& tileMap) {
    const Room* entrance = dungeon.getRoom(dungeon.entranceId());
    const Room* exitRoom = dungeon.getRoom(dungeon.exitId());
    if (!entrance || !exitRoom) {
        return false;
    }

    if (!tileMap.isReachable(entrance->physicalCenter(), exitRoom->physicalCenter())) {
        return false;
    }

    if (!tileMap.allRoomTilesReachable(entrance->physicalCenter())) {
        return false;
    }

    return true;
}

} // namespace

void runDungeonConsoleTest() {
    constexpr int kDungeonCount = 5;
    int logicalPassed = 0;
    int physicalPassed = 0;

    std::cout << "=== Prueba de generacion procedural de mazmorras ===\n\n";

    for (int i = 0; i < kDungeonCount; ++i) {
        GeneratorConfig config;
        config.rows = 12;
        config.cols = 16;
        config.mainPathMinLength = 6;
        config.mainPathMaxLength = 10;
        config.maxBranchLength = 3;
        config.seed = 0;

        LayoutConfig layoutConfig;
        layoutConfig.corridorWidth = 3;
        layoutConfig.seed = 0;

        DungeonGenerator generator;
        Dungeon dungeon = generator.generate(config);

        const bool logicalOk = validateDungeon(dungeon);
        logicalPassed += logicalOk ? 1 : 0;

        DungeonLayoutBuilder layoutBuilder;
        TileMap tileMap = layoutBuilder.build(dungeon, layoutConfig);

        const bool physicalOk = logicalOk && validatePhysicalLayout(dungeon, tileMap);
        physicalPassed += physicalOk ? 1 : 0;

        std::cout << "--- Dungeon #" << (i + 1)
                  << (logicalOk ? " [LOGICO OK]" : " [LOGICO FAIL]")
                  << (physicalOk ? " [FISICO OK]" : " [FISICO FAIL]") << " ---\n";

        DungeonPrinter::printAll(std::cout, dungeon);
        TileMapPrinter::printCompact(std::cout, tileMap);
    }

    std::cout << "=== Resultado logico: " << logicalPassed << "/" << kDungeonCount << " ===\n";
    std::cout << "=== Resultado fisico: " << physicalPassed << "/" << kDungeonCount << " ===\n";

    if (logicalPassed == kDungeonCount && physicalPassed == kDungeonCount) {
        std::cout << "Todas las pruebas pasaron.\n";
    } else {
        std::cout << "Algunas pruebas fallaron.\n";
    }
}

} // namespace world
