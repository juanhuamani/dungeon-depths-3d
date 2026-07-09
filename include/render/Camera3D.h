#pragma once

#include <glm/glm.hpp>

namespace render {

class Camera3D {
public:
    glm::mat4 viewProjection(float viewportWidth, float viewportHeight) const;

    void setTarget(const glm::vec3& target) { target_ = target; }
    const glm::vec3& target() const { return target_; }

    void setDistance(float distance) { distance_ = glm::clamp(distance, 5.0f, 500.0f); }
    float distance() const { return distance_; }

    void setPitch(float pitchDegrees) { pitch_ = glm::clamp(pitchDegrees, 10.0f, 89.0f); }
    void setYaw(float yawDegrees) { yaw_ = yawDegrees; }

    void pan(const glm::vec2& delta);
    void zoomBy(float factor);
    void centerOn(const glm::vec2& worldCenterXZ);

private:
    glm::vec3 target_{0.0f, 0.0f, 0.0f};
    float distance_ = 60.0f;
    float pitch_ = 45.0f;
    float yaw_ = -45.0f;
};

} // namespace render
