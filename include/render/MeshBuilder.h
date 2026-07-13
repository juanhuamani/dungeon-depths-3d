#pragma once

#include <glad/gl.h>
#include <glm/glm.hpp>

#include <vector>

namespace render {

struct TileVertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
    glm::vec4 color;
};

void addAxisAlignedBox(std::vector<TileVertex>& vertices,
                       std::vector<GLuint>& indices,
                       const glm::vec3& minCorner,
                       const glm::vec3& maxCorner,
                       const glm::vec4& color);

} // namespace render
