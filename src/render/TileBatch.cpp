#include "render/TileBatch.h"

#include "render/MeshBuilder.h"
#include "render/TileColors.h"
#include "render/TileCoordinates.h"

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
    const float pillar = tileSize * kDoorPillarRatio;
    const float yBase = kFloorHeight;

    addFloorSlab(vertices, indices, origin, tileSize, color);

    addAxisAlignedBox(
        vertices,
        indices,
        origin + glm::vec3(0.0f, yBase, 0.0f),
        origin + glm::vec3(pillar, kDoorHeight, tileSize),
        color);

    addAxisAlignedBox(
        vertices,
        indices,
        origin + glm::vec3(tileSize - pillar, yBase, 0.0f),
        origin + glm::vec3(tileSize, kDoorHeight, tileSize),
        color);

    addAxisAlignedBox(
        vertices,
        indices,
        origin + glm::vec3(0.0f, kDoorHeight, 0.0f),
        origin + glm::vec3(tileSize, kWallHeight, tileSize),
        color);
}

void buildTileGeometry(std::vector<TileVertex>& vertices,
                       std::vector<GLuint>& indices,
                       world::TileType type,
                       const glm::vec3& origin,
                       float tileSize) {
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
            break;
        case world::TileType::Wall:
            addFloorSlab(vertices, indices, origin, tileSize,
                         glm::vec4(color.r * 0.6f, color.g * 0.6f, color.b * 0.6f, 1.0f));
            addWallBlock(vertices, indices, origin, tileSize, color);
            break;
        case world::TileType::Door:
            addDoorFrame(vertices, indices, origin, tileSize, color);
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
            const world::TileType type = tileMap.at({row, col});
            if (type == world::TileType::Empty) {
                continue;
            }

            const glm::vec3 origin = tileOrigin({row, col}, tileSize);
            buildTileGeometry(vertices_, indices_, type, origin, tileSize);
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
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(0));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(sizeof(glm::vec3)));

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
