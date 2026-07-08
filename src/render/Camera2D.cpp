#include "render/Camera2D.h"

#include <glm/gtc/matrix_transform.hpp>

namespace render {

glm::mat4 Camera2D::viewProjection(float viewportWidth, float viewportHeight) const {
    const float aspect = viewportWidth / viewportHeight;
    const float baseHeight = 40.0f;
    const float halfHeight = (baseHeight / zoom_) * 0.5f;
    const float halfWidth = halfHeight * aspect;

    const glm::vec3 eye(position_.x, 80.0f, position_.y);
    const glm::vec3 center(position_.x, 0.0f, position_.y);
    const glm::mat4 view = glm::lookAt(eye, center, glm::vec3(0.0f, 0.0f, -1.0f));
    const glm::mat4 projection = glm::ortho(-halfWidth, halfWidth, -halfHeight, halfHeight, 0.1f, 200.0f);
    return projection * view;
}

void Camera2D::zoomBy(float factor) {
    if (factor <= 0.0f) {
        return;
    }
    setZoom(zoom_ * factor);
}

} // namespace render
