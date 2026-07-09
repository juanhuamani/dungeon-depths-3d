#pragma once

#include <glm/glm.hpp>

namespace render {

class Camera2D {
public:
    glm::mat4 viewProjection(float viewportWidth, float viewportHeight) const;

    void setTileSize(float tileSize) { tileSize_ = tileSize; }
    float tileSize() const { return tileSize_; }

    void setPosition(const glm::vec2& position) { position_ = position; }
    const glm::vec2& position() const { return position_; }

    void setZoom(float zoom) { zoom_ = glm::clamp(zoom, 0.05f, 20.0f); }
    float zoom() const { return zoom_; }

    void pan(const glm::vec2& delta) { position_ += delta; }
    void zoomBy(float factor);
    void centerOn(const glm::vec2& worldCenterXZ) { position_ = worldCenterXZ; }

private:
    glm::vec2 position_{0.0f}; // X y Z del plano del suelo
    float zoom_ = 1.0f;
    float tileSize_ = 1.0f;
};

} // namespace render
