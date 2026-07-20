#pragma once

#include "enemies/Enemy.h"
#include <vector>
#include <memory>
#include <glm/glm.hpp>

namespace world { class Dungeon; }
class ItemManager;

class EnemyManager {
public:
    void init(game::Game* game, Player* player, EnemyProjectileManager* projManager, ItemManager* itemManager);
    
    // Genera enemigos basándose en el laberinto y el nivel actual (dificultad)
    void spawnFromDungeon(const world::Dungeon& dungeon, int levelIndex, float tileSize);
    
    void update(float deltaTime);
    void resetForNewLevel();

    std::vector<std::unique_ptr<Enemy>>& getEnemies() { return m_enemies; }

private:
    void spawnEnemy(EnemyType type, const glm::vec3& worldPos, int levelIndex);
    
    game::Game* m_game = nullptr;
    Player* m_player = nullptr;
    EnemyProjectileManager* m_projManager = nullptr;
    ItemManager* m_itemManager = nullptr;
    
    std::vector<std::unique_ptr<Enemy>> m_enemies;
};
