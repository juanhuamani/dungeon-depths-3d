#include "render/TileBatch.h"

#include "render/MeshBuilder.h"
#include "render/TileColors.h"
#include "render/TileCoordinates.h"
#include "world/DungeonTypes.h"

namespace render {

namespace {

using namespace TileMeshConfig;

void addFloorSlab(std::vector<TileVertex>& vertices,
                  std::vector<GLuint>& indices,
                  const glm::vec3& origin,
                  float tileSize,
                  const glm::vec4& color) {
    addAxisAlignedBox(
        vertices,
        indices,
        origin,
        origin + glm::vec3(tileSize, kFloorHeight, tileSize),
        color);
}

void addCeilingSlab(std::vector<TileVertex>& vertices,
                    std::vector<GLuint>& indices,
                    const glm::vec3& origin,
                    float tileSize,
                    const glm::vec4& color) {
    addAxisAlignedBox(
        vertices,
        indices,
        origin + glm::vec3(0.0f, kWallHeight, 0.0f),
        origin + glm::vec3(tileSize, kWallHeight + kCeilingThickness, tileSize),
        color);
}

void addWallBlock(std::vector<TileVertex>& vertices,
                  std::vector<GLuint>& indices,
                  const glm::vec3& origin,
                  float tileSize,
                  const glm::vec4& color) {
    addAxisAlignedBox(
        vertices,
        indices,
        origin + glm::vec3(0.0f, kFloorHeight, 0.0f),
        origin + glm::vec3(tileSize, kWallHeight, tileSize),
        color);
}



void addDoorFrame(std::vector<TileVertex>& vertices,
                  std::vector<GLuint>& indices,
                  const glm::vec3& origin,
                  float tileSize,
                  const glm::vec4& color) {
    addFloorSlab(vertices, indices, origin, tileSize, color);

    // Añadimos solo el dintel (parte superior del marco) para formar un arco, 
    // sin pilares laterales que estorben en puertas anchas.
    addAxisAlignedBox(
        vertices,
        indices,
        origin + glm::vec3(0.0f, kDoorHeight, 0.0f),
        origin + glm::vec3(tileSize, kWallHeight, tileSize),
        color);
}

glm::vec4 darkened(const glm::vec4& color, float factor) {
    return {color.r * factor, color.g * factor, color.b * factor, color.a};
}

void buildTileGeometry(std::vector<TileVertex>& vertices,
                       std::vector<GLuint>& indices,
                       const world::TileMap& tileMap,
                       world::TilePos pos,
                       const glm::vec3& origin,
                       float tileSize) {
    const world::TileType type = tileMap.at(pos);
    const glm::vec4 color = colorForTileType(type);

    switch (type) {
        case world::TileType::Room:
        case world::TileType::Corridor:
        case world::TileType::Entrance:
        case world::TileType::Exit:
        case world::TileType::Treasure:
        case world::TileType::Enemy:
        case world::TileType::Key:
            addFloorSlab(vertices, indices, origin, tileSize, color);
            addCeilingSlab(vertices, indices, origin, tileSize, darkened(color, 0.7f));
            break;
        case world::TileType::Wall: {
            const glm::vec4 wallColor = wallColorForNeighbors(tileMap, pos);
            addFloorSlab(vertices, indices, origin, tileSize, darkened(wallColor, 0.6f));
            addWallBlock(vertices, indices, origin, tileSize, wallColor);
            break;
        }
        case world::TileType::Door:
            addDoorFrame(
                vertices,
                indices,
                origin,
                tileSize,
                color);
            addCeilingSlab(vertices, indices, origin, tileSize, darkened(color, 0.7f));
            break;
        default:
            break;
    }
}

} // namespace

TileBatch::~TileBatch() = default;

void TileBatch::destroyGpu() {
    clearGpu();
}

void TileBatch::clearGpu() {
    if (ebo_ != 0) {
        glDeleteBuffers(1, &ebo_);
        ebo_ = 0;
    }
    if (vbo_ != 0) {
        glDeleteBuffers(1, &vbo_);
        vbo_ = 0;
    }
    if (vao_ != 0) {
        glDeleteVertexArrays(1, &vao_);
        vao_ = 0;
    }
    indexCount_ = 0;
}

void TileBatch::buildFromTileMap(const world::TileMap& tileMap, float tileSize) {
    vertices_.clear();
    indices_.clear();

    for (int row = 0; row < tileMap.rows(); ++row) {
        for (int col = 0; col < tileMap.cols(); ++col) {
            const world::TilePos pos{row, col};
            const world::TileType type = tileMap.at(pos);
            if (type == world::TileType::Empty) {
                continue;
            }

            const glm::vec3 origin = tileOrigin(pos, tileSize);
            buildTileGeometry(vertices_, indices_, tileMap, pos, origin, tileSize);
        }
    }

    uploadBuffers();
}

void TileBatch::uploadBuffers() {
    clearGpu();

    if (vertices_.empty() || indices_.empty()) {
        return;
    }

    indexCount_ = static_cast<GLsizei>(indices_.size());

    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);
    glGenBuffers(1, &ebo_);

    glBindVertexArray(vao_);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER,
                 static_cast<GLsizeiptr>(vertices_.size() * sizeof(TileVertex)),
                 vertices_.data(),
                 GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 static_cast<GLsizeiptr>(indices_.size() * sizeof(GLuint)),
                 indices_.data(),
                 GL_STATIC_DRAW);

    constexpr GLsizei stride = static_cast<GLsizei>(sizeof(TileVertex));
    // position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(offsetof(TileVertex, position)));
    // normal
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(offsetof(TileVertex, normal)));
    // texCoords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(offsetof(TileVertex, texCoords)));
    // color
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(offsetof(TileVertex, color)));

    glBindVertexArray(0);
}

void TileBatch::draw() const {
    if (indexCount_ == 0 || vao_ == 0) {
        return;
    }
    glBindVertexArray(vao_);
    glDrawElements(GL_TRIANGLES, indexCount_, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

} // namespace render
