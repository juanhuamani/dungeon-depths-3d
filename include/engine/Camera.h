#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum class CameraMode { FIRST_PERSON, THIRD_PERSON };

class Camera {
public:
    Camera();

    void processMouseMovement(float xOffset, float yOffset);
    void toggleMode();
    void setMode(CameraMode mode) { m_mode = mode; }

    glm::mat4 getViewMatrix(const glm::vec3& playerPos) const;
    glm::vec3 getCameraPosition(const glm::vec3& playerPos) const;
    glm::vec3 getForward() const;
    glm::vec3 getRight() const;
    glm::vec3 getLookDirection() const;

    float      getYaw() const   { return m_yaw; }
    float      getPitch() const { return m_pitch; }
    CameraMode getMode() const  { return m_mode; }

    float sensitivity = 0.1f;
    float eyeHeight   = 1.6f;
    float tpDistance   = 5.0f;
    float tpHeight    = 2.5f;
    float pitchMin    = -60.0f;
    float pitchMax    =  60.0f;

private:
    CameraMode m_mode  = CameraMode::FIRST_PERSON;
    float      m_yaw   = -90.0f;
    float      m_pitch =  0.0f;

    void clampPitch();
};
