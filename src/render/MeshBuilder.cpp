#include "render/MeshBuilder.h"

namespace render {

namespace {

void addFace(std::vector<TileVertex>& vertices,
             std::vector<GLuint>& indices,
             const glm::vec3& a,
             const glm::vec3& b,
             const glm::vec3& c,
             const glm::vec3& d,
             const glm::vec4& color) {
    const GLuint base = static_cast<GLuint>(vertices.size());
    vertices.push_back({a, color});
    vertices.push_back({b, color});
    vertices.push_back({c, color});
    vertices.push_back({d, color});

    indices.push_back(base + 0);
    indices.push_back(base + 1);
    indices.push_back(base + 2);
    indices.push_back(base + 2);
    indices.push_back(base + 3);
    indices.push_back(base + 0);
}

} // namespace

void addAxisAlignedBox(std::vector<TileVertex>& vertices,
                       std::vector<GLuint>& indices,
                       const glm::vec3& minCorner,
                       const glm::vec3& maxCorner,
                       const glm::vec4& color) {
    const glm::vec3& mn = minCorner;
    const glm::vec3& mx = maxCorner;

    const glm::vec3 v000{mn.x, mn.y, mn.z};
    const glm::vec3 v001{mn.x, mn.y, mx.z};
    const glm::vec3 v010{mn.x, mx.y, mn.z};
    const glm::vec3 v011{mn.x, mx.y, mx.z};
    const glm::vec3 v100{mx.x, mn.y, mn.z};
    const glm::vec3 v101{mx.x, mn.y, mx.z};
    const glm::vec3 v110{mx.x, mx.y, mn.z};
    const glm::vec3 v111{mx.x, mx.y, mx.z};

    addFace(vertices, indices, v010, v110, v111, v011, color); // +X
    addFace(vertices, indices, v101, v001, v011, v111, color); // +Z
    addFace(vertices, indices, v100, v000, v010, v110, color); // -X
    addFace(vertices, indices, v000, v100, v101, v001, color); // -Z
    addFace(vertices, indices, v011, v111, v110, v010, color); // +Y top
    addFace(vertices, indices, v000, v001, v101, v100, color); // -Y bottom
}

} // namespace render
