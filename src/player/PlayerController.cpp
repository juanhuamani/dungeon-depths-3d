#include "player/PlayerController.h"
#include "engine/InputManager.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

PlayerController::PlayerController(Player& player, Camera& camera, ProjectileManager& projectileManager)
    : m_player(player)
    , m_camera(camera)
    , m_projectiles(projectileManager)
{
}

void PlayerController::handleInput(float deltaTime)
{
    m_shotFired = false;

    if (!m_player.isAlive())
        return;

    handleMovement(deltaTime);
    handleAttacks();

    m_player.setYaw(m_camera.getYaw());
}

void PlayerController::handleMouseMovement(float xOffset, float yOffset)
{
    m_camera.processMouseMovement(xOffset, yOffset);
}

void PlayerController::handleMovement(float deltaTime)
{
    deltaTime = std::min(deltaTime, 0.05f);

    glm::vec3 forward = m_camera.getForward();
    glm::vec3 right = m_camera.getRight();

    glm::vec3 direction{0.0f};

    if (InputManager::isKeyDown(GLFW_KEY_W))
        direction += forward;
    if (InputManager::isKeyDown(GLFW_KEY_S))
        direction -= forward;
    if (InputManager::isKeyDown(GLFW_KEY_D))
        direction += right;
    if (InputManager::isKeyDown(GLFW_KEY_A))
        direction -= right;

    if (glm::length(direction) > 0.001f)
        direction = glm::normalize(direction);

    glm::vec3 velocity = direction * m_player.getSpeed();
    m_player.move(velocity, deltaTime);
}

void PlayerController::handleAttacks()
{
    if (InputManager::isMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT))
    {
        m_player.attackMelee();
    }

    if (InputManager::isMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT))
    {
        if (m_player.attackRanged())
        {
            m_shotFired = true;
            glm::vec3 dir = m_camera.getLookDirection();

            glm::vec3 origin = m_player.transform.position;
            origin.y += 1.2f;
            m_projectiles.shoot(origin, dir);
        }
    }
}
