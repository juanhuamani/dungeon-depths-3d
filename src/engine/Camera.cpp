#include "engine/Camera.h"

#include <algorithm>
#include <cmath>

Camera::Camera()
    : m_mode(CameraMode::FIRST_PERSON)
    , m_yaw(-90.0f)
    , m_pitch(0.0f)
{
}

void Camera::processMouseMovement(float xOffset, float yOffset)
{
    m_yaw   += xOffset * sensitivity;
    m_pitch += yOffset * sensitivity;
    clampPitch();
    if (m_yaw > 360.0f)  m_yaw -= 360.0f;
    if (m_yaw < 0.0f)    m_yaw += 360.0f;
}

void Camera::toggleMode()
{
    m_mode = (m_mode == CameraMode::FIRST_PERSON)
             ? CameraMode::THIRD_PERSON
             : CameraMode::FIRST_PERSON;
}

glm::mat4 Camera::getViewMatrix(const glm::vec3& playerPos) const
{
    glm::vec3 camPos = getCameraPosition(playerPos);
    glm::vec3 target;

    if (m_mode == CameraMode::FIRST_PERSON)
        target = camPos + getLookDirection();
    else
        target = playerPos + glm::vec3(0.0f, eyeHeight * 0.7f, 0.0f);

    return glm::lookAt(camPos, target, glm::vec3(0.0f, 1.0f, 0.0f));
}

glm::vec3 Camera::getCameraPosition(const glm::vec3& playerPos) const
{
    if (m_mode == CameraMode::FIRST_PERSON)
        return playerPos + glm::vec3(0.0f, eyeHeight, 0.0f);

    glm::vec3 forward = getLookDirection();
    return playerPos + glm::vec3(0.0f, tpHeight, 0.0f) - forward * tpDistance;
}

glm::vec3 Camera::getForward() const
{
    float yawRad = glm::radians(m_yaw);
    return glm::normalize(glm::vec3(cos(yawRad), 0.0f, sin(yawRad)));
}

glm::vec3 Camera::getRight() const
{
    float yawRad = glm::radians(m_yaw + 90.0f);
    return glm::normalize(glm::vec3(cos(yawRad), 0.0f, sin(yawRad)));
}

glm::vec3 Camera::getLookDirection() const
{
    float yawRad   = glm::radians(m_yaw);
    float pitchRad = glm::radians(m_pitch);
    return glm::normalize(glm::vec3(
        cos(pitchRad) * cos(yawRad),
        sin(pitchRad),
        cos(pitchRad) * sin(yawRad)
    ));
}

void Camera::clampPitch()
{
    m_pitch = std::clamp(m_pitch, pitchMin, pitchMax);
}
