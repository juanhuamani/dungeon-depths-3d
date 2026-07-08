#pragma once

#include "render/Camera2D.h"
#include "render/Camera3D.h"

#include <glm/glm.hpp>

namespace render {

enum class ViewMode {
    TopDown,
    Perspective3D
};

class MapCamera {
public:
    ViewMode mode() const { return mode_; }
    void setMode(ViewMode mode) { mode_ = mode; }

    Camera2D& camera2D() { return camera2D_; }
    Camera3D& camera3D() { return camera3D_; }
    const Camera2D& camera2D() const { return camera2D_; }
    const Camera3D& camera3D() const { return camera3D_; }

    glm::mat4 viewProjection(float viewportWidth, float viewportHeight) const;
    void pan(const glm::vec2& delta);
    void zoomBy(float factor);
    void centerOn(const glm::vec2& worldCenterXZ);
    void fitToMap(const glm::vec2& worldCenterXZ, float mapWidth, float mapHeight,
                  float viewportWidth, float viewportHeight);

private:
    ViewMode mode_ = ViewMode::TopDown;
    Camera2D camera2D_;
    Camera3D camera3D_;
};

} // namespace render
