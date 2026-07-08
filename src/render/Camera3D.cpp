#include "render/Camera3D.h"

#include <glm/gtc/matrix_transform.hpp>

namespace render {

glm::mat4 Camera3D::viewProjection(float viewportWidth, float viewportHeight) const {
    const float aspect = viewportWidth / viewportHeight;
    const glm::mat4 projection = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 2000.0f);

    const float pitchRad = glm::radians(pitch_);
    const float yawRad = glm::radians(yaw_);
    const float horizontal = distance_ * glm::cos(pitchRad);
    const float eyeX = target_.x + horizontal * glm::cos(yawRad);
    const float eyeZ = target_.z + horizontal * glm::sin(yawRad);
    const float eyeY = target_.y + distance_ * glm::sin(pitchRad);

    const glm::vec3 eye(eyeX, eyeY, eyeZ);
    const glm::mat4 view = glm::lookAt(eye, target_, glm::vec3(0.0f, 1.0f, 0.0f));
    return projection * view;
}

void Camera3D::pan(const glm::vec2& delta) {
    target_.x += delta.x;
    target_.z += delta.y;
}

void Camera3D::zoomBy(float factor) {
    if (factor <= 0.0f) {
        return;
    }
    setDistance(distance_ / factor);
}

void Camera3D::centerOn(const glm::vec2& worldCenterXZ) {
    target_ = glm::vec3(worldCenterXZ.x, 0.0f, worldCenterXZ.y);
}

} // namespace render
