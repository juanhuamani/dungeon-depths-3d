#include "render/TileBatch.h"

#include "render/TileColors.h"
#include "render/TileCoordinates.h"

namespace render {

TileBatch::~TileBatch() {
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

            const glm::vec4 color = colorForTileType(type);
            const glm::vec2 topLeft = tileToWorld({row, col}, tileSize);
            const glm::vec2 bottomLeft{topLeft.x, topLeft.y - tileSize};
            const glm::vec2 bottomRight{topLeft.x + tileSize, topLeft.y - tileSize};
            const glm::vec2 topRight{topLeft.x + tileSize, topLeft.y};

            const GLuint baseIndex = static_cast<GLuint>(vertices_.size());
            vertices_.push_back({{bottomLeft.x, bottomLeft.y, 0.0f}, color});
            vertices_.push_back({{bottomRight.x, bottomRight.y, 0.0f}, color});
            vertices_.push_back({{topRight.x, topRight.y, 0.0f}, color});
            vertices_.push_back({{topLeft.x, topLeft.y, 0.0f}, color});

            indices_.push_back(baseIndex + 0);
            indices_.push_back(baseIndex + 1);
            indices_.push_back(baseIndex + 2);
            indices_.push_back(baseIndex + 2);
            indices_.push_back(baseIndex + 3);
            indices_.push_back(baseIndex + 0);
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
