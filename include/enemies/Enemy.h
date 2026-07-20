#pragma once

#include "engine/GameObject.h"
#include <memory>

class IEnemyBehavior;
class Player;
class EnemyProjectileManager;
namespace game { class Game; }

enum class EnemyType {
    CHASER,
    SHOOTER,
    CHARGER,
    BOSS
};

class Enemy : public GameObject {
public:
    Enemy(EnemyType type, const glm::vec3& startPos, int maxHealth, float speed, int contactDamage, 
          game::Game* game, Player* player, EnemyProjectileManager* projManager);
    ~Enemy() override;

    void update(float deltaTime) override;

    void takeDamage(int amount);
    bool isAlive() const { return m_health > 0; }

    void setBehavior(std::unique_ptr<IEnemyBehavior> behavior);

    EnemyType getType() const { return m_type; }
    int getHealth() const { return m_health; }
    int getMaxHealth() const { return m_maxHealth; }
    float getSpeed() const { return m_speed; }
    int getContactDamage() const { return m_contactDamage; }
    
    game::Game* getGame() const { return m_game; }
    Player* getPlayer() const { return m_player; }
    EnemyProjectileManager* getProjectileManager() const { return m_projManager; }

    // Helpers para comportamientos
    void moveWithCollision(const glm::vec3& velocity, float deltaTime);

    // Color para DebugRenderer (opcional, para visualización antes de modelos 3D)
    glm::vec3 color{1.0f, 0.0f, 0.0f};

    static constexpr glm::vec3 HALF_SIZE{0.35f, 0.9f, 0.35f};

private:
    EnemyType m_type;
    int m_health;
    int m_maxHealth;
    float m_speed;
    int m_contactDamage;
    
    game::Game* m_game;
    Player* m_player;
    EnemyProjectileManager* m_projManager;

    std::unique_ptr<IEnemyBehavior> m_behavior;
};
