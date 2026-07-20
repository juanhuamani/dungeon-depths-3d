#include "enemies/BossBehavior.h"
#include "enemies/Enemy.h"
#include "enemies/EnemyProjectileManager.h"
#include "player/Player.h"
#include <cmath>

void BossBehavior::update(Enemy& enemy, float deltaTime) {
    enemy.color = glm::vec3(0.8f, 0.1f, 0.8f); // Morado de jefe
    
    Player* player = enemy.getPlayer();
    if (!player || !player->isAlive()) return;

    if (m_damageTimer > 0.0f) {
        m_damageTimer -= deltaTime;
    }

    glm::vec3 toPlayer = player->transform.position - enemy.transform.position;
    toPlayer.y = 0.0f;
    float dist = glm::length(toPlayer);

    // Moverse lentamente hacia el jugador si está lejos
    if (dist > 5.0f && dist < 15.0f) {
        enemy.moveWithCollision(glm::normalize(toPlayer) * (enemy.getSpeed() * 0.5f), deltaTime);
    }

    m_phaseTimer -= deltaTime;
    if (m_phaseTimer <= 0.0f) {
        m_phase = (m_phase + 1) % 2;
        m_phaseTimer = 3.0f;
    }

    m_shootTimer -= deltaTime;
    if (m_shootTimer <= 0.0f) {
        if (enemy.getProjectileManager()) {
            if (m_phase == 0) {
                // Dispara hacia el jugador y dos ligeramente desviados
                enemy.getProjectileManager()->shoot(enemy.transform.position, toPlayer);
                
                glm::vec3 dir2 = glm::vec3(toPlayer.x * cos(0.3f) - toPlayer.z * sin(0.3f), 0.0f,
                                           toPlayer.x * sin(0.3f) + toPlayer.z * cos(0.3f));
                glm::vec3 dir3 = glm::vec3(toPlayer.x * cos(-0.3f) - toPlayer.z * sin(-0.3f), 0.0f,
                                           toPlayer.x * sin(-0.3f) + toPlayer.z * cos(-0.3f));
                                           
                enemy.getProjectileManager()->shoot(enemy.transform.position, dir2);
                enemy.getProjectileManager()->shoot(enemy.transform.position, dir3);
                
                m_shootTimer = 1.0f;
            } else {
                // Anillo de fuego (8 direcciones)
                for (int i = 0; i < 8; ++i) {
                    float angle = (i * 45.0f) * 3.14159f / 180.0f;
                    glm::vec3 dir(std::cos(angle), 0.0f, std::sin(angle));
                    enemy.getProjectileManager()->shoot(enemy.transform.position, dir);
                }
                m_shootTimer = 1.5f;
            }
        }
    }

    if (m_damageTimer <= 0.0f && enemy.collider.intersects(player->collider)) {
        player->takeDamage(enemy.getContactDamage());
        m_damageTimer = 1.0f;
    }
}
