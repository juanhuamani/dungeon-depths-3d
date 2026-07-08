#include "render/MapCamera.h"

#include <algorithm>
#include <cmath>

namespace render {

glm::mat4 MapCamera::viewProjection(float viewportWidth, float viewportHeight) const {
    if (mode_ == ViewMode::Perspective3D) {
        return camera3D_.viewProjection(viewportWidth, viewportHeight);
    }
    return camera2D_.viewProjection(viewportWidth, viewportHeight);
}

void MapCamera::pan(const glm::vec2& delta) {
    camera2D_.pan(delta);
    camera3D_.pan(delta);
}

void MapCamera::zoomBy(float factor) {
    camera2D_.zoomBy(factor);
    camera3D_.zoomBy(factor);
}

void MapCamera::centerOn(const glm::vec2& worldCenterXZ) {
    camera2D_.centerOn(worldCenterXZ);
    camera3D_.centerOn(worldCenterXZ);
}

void MapCamera::fitToMap(const glm::vec2& worldCenterXZ, float mapWidth, float mapHeight,
                         float viewportWidth, float viewportHeight) {
    centerOn(worldCenterXZ);

    const float aspect = viewportWidth / viewportHeight;
    const float baseHeight = 40.0f;

    const float zoomForHeight = baseHeight / std::max(mapHeight * 1.4f, 1.0f);
    const float zoomForWidth = (baseHeight * aspect) / std::max(mapWidth * 1.4f, 1.0f);
    camera2D_.setZoom(std::min(zoomForHeight, zoomForWidth));

    const float mapDiagonal = std::sqrt(mapWidth * mapWidth + mapHeight * mapHeight);
    camera3D_.setDistance(std::max(mapDiagonal * 1.2f, 30.0f));
}

} // namespace render
