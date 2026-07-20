#include "enemies/EnemyManager.h"
#include "enemies/ChaserBehavior.h"
#include "enemies/ShooterBehavior.h"
#include "enemies/ChargerBehavior.h"
#include "enemies/BossBehavior.h"
#include "world/Dungeon.h"
#include "player/ItemManager.h"
#include <cstdlib>

void EnemyManager::init(game::Game* game, Player* player, EnemyProjectileManager* projManager, ItemManager* itemManager) {
    m_game = game;
    m_player = player;
    m_projManager = projManager;
    m_itemManager = itemManager;
}

void EnemyManager::spawnFromDungeon(const world::Dungeon& dungeon, int levelIndex, float tileSize) {
    resetForNewLevel();
    
    // Aumentar la cantidad de enemigos base por sala dependiendo del nivel
    int minEnemies = 1 + (levelIndex / 2);
    int maxEnemies = 2 + levelIndex;
    
    for (const auto& room : dungeon.rooms()) {
        // No spawnear en la entrada
        if (room.id() == dungeon.entranceId()) continue;
        
        // Si es la salida, spawnear al jefe
        if (room.id() == dungeon.exitId()) {
            glm::vec3 bossPos(
                (room.bounds().col + room.bounds().width / 2.0f) * tileSize,
                0.0f,
                (room.bounds().row + room.bounds().height / 2.0f) * tileSize
            );
            spawnEnemy(EnemyType::BOSS, bossPos, levelIndex);
            continue; // Evitar spawnear otros enemigos normales en la sala del jefe
        }
        
        int numEnemies = minEnemies + (std::rand() % (maxEnemies - minEnemies + 1));
        
        for (int i = 0; i < numEnemies; ++i) {
            float cx = room.bounds().col + room.bounds().width / 2.0f;
            float cz = room.bounds().row + room.bounds().height / 2.0f;
            
            // Spawnear en un área pequeña alrededor del centro de la sala
            float rx = cx - 1.5f + (std::rand() % 30) / 10.0f;
            float rz = cz - 1.5f + (std::rand() % 30) / 10.0f;
            glm::vec3 pos(rx * tileSize, 0.0f, rz * tileSize);
            
            // Probabilidades: 50% Chaser, 30% Shooter, 20% Charger (a partir de nivel 2)
            int randType = std::rand() % 100;
            EnemyType type = EnemyType::CHASER;
            
            if (randType > 50 && randType <= 80) type = EnemyType::SHOOTER;
            else if (randType > 80 && levelIndex >= 1) type = EnemyType::CHARGER;
            
            spawnEnemy(type, pos, levelIndex);
        }
    }
}

void EnemyManager::spawnEnemy(EnemyType type, const glm::vec3& worldPos, int levelIndex) {
    // Escalar stats por nivel
    float hpMult = 1.0f + (levelIndex * 0.2f);
    float dmgMult = 1.0f + (levelIndex * 0.1f);
    
    std::unique_ptr<Enemy> enemy;
    
    if (type == EnemyType::CHASER) {
        enemy = std::make_unique<Enemy>(type, worldPos, 3 * hpMult, 3.5f, 1 * dmgMult, m_game, m_player, m_projManager);
        enemy->setBehavior(std::make_unique<ChaserBehavior>());
    } else if (type == EnemyType::SHOOTER) {
        enemy = std::make_unique<Enemy>(type, worldPos, 2 * hpMult, 2.5f, 1, m_game, m_player, m_projManager);
        enemy->setBehavior(std::make_unique<ShooterBehavior>());
    } else if (type == EnemyType::CHARGER) {
        // Reducimos el daño base del charger (antes 2, ahora 1) para que la embestida baje 2 en vez de 4.
        enemy = std::make_unique<Enemy>(type, worldPos, 5 * hpMult, 2.0f, 1 * dmgMult, m_game, m_player, m_projManager);
        enemy->setBehavior(std::make_unique<ChargerBehavior>());
    } else if (type == EnemyType::BOSS) {
        enemy = std::make_unique<Enemy>(type, worldPos, 25 * hpMult, 2.0f, 2 * dmgMult, m_game, m_player, m_projManager);
        enemy->setBehavior(std::make_unique<BossBehavior>());
        enemy->transform.scale = glm::vec3(2.0f); // Jefe más grande
        enemy->syncCollider(Enemy::HALF_SIZE * 2.0f);
    }
    
    if (enemy) m_enemies.push_back(std::move(enemy));
}

void EnemyManager::update(float deltaTime) {
    for (auto it = m_enemies.begin(); it != m_enemies.end();) {
        if (!(*it)->isAlive()) {
            // Drop de items
            if (m_itemManager) {
                m_itemManager->spawnDrop((*it)->transform.position);
            }
            it = m_enemies.erase(it);
        } else {
            (*it)->update(deltaTime);
            ++it;
        }
    }
}

void EnemyManager::resetForNewLevel() {
    m_enemies.clear();
}
