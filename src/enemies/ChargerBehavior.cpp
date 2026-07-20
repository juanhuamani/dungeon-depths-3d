#include "enemies/ChargerBehavior.h"
#include "enemies/Enemy.h"
#include "player/Player.h"
#include <cstdlib>

void ChargerBehavior::update(Enemy& enemy, float deltaTime) {
    Player* player = enemy.getPlayer();
    if (!player || !player->isAlive()) return;

    if (m_damageTimer > 0.0f) {
        m_damageTimer -= deltaTime;
    }

    m_timer -= deltaTime;

    switch (m_state) {
        case State::WANDERING:
            enemy.color = glm::vec3(0.0f, 0.8f, 1.0f); // Azul / normal
            if (m_timer <= 0.0f) {
                m_state = State::WINDUP;
                m_timer = 1.0f; // 1 segundo de preparación
            }
            break;

        case State::WINDUP:
            // Parpadeo naranja/rojo y dejar de moverse
            if (std::fmod(m_timer, 0.2f) > 0.1f) {
                enemy.color = glm::vec3(1.0f, 0.5f, 0.0f);
            } else {
                enemy.color = glm::vec3(1.0f, 0.0f, 0.0f);
            }
            
            if (m_timer <= 0.0f) {
                m_state = State::CHARGING;
                m_timer = 0.8f; // Tiempo de carga
                
                glm::vec3 toPlayer = player->transform.position - enemy.transform.position;
                toPlayer.y = 0.0f;
                m_chargeDirection = glm::normalize(toPlayer);
            }
            break;

        case State::CHARGING:
            enemy.color = glm::vec3(1.0f, 0.0f, 0.0f); // Rojo fuego
            enemy.moveWithCollision(m_chargeDirection * (enemy.getSpeed() * 3.0f), deltaTime);
            
            if (m_timer <= 0.0f) {
                m_state = State::RECOVERING;
                m_timer = 1.5f; // Tiempo de recuperación
            }
            break;

        case State::RECOVERING:
            enemy.color = glm::vec3(0.3f, 0.3f, 0.3f); // Gris mareado
            if (m_timer <= 0.0f) {
                m_state = State::WANDERING;
                m_timer = 3.0f;
            }
            break;
    }

    // Colisión por contacto
    if (m_damageTimer <= 0.0f && enemy.collider.intersects(player->collider)) {
        int damage = (m_state == State::CHARGING) ? enemy.getContactDamage() * 2 : enemy.getContactDamage();
        player->takeDamage(damage);
        m_damageTimer = 1.0f;
        
        if (m_state == State::CHARGING) {
            m_state = State::RECOVERING;
            m_timer = 1.5f;
        }
    }
}
