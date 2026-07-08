#pragma once

#include "world/TileMap.h"

#include <glad/gl.h>
#include <glm/glm.hpp>

#include <vector>

namespace render {

struct TileVertex {
    glm::vec3 position;
    glm::vec4 color;
};

class TileBatch {
public:
    TileBatch() = default;
    ~TileBatch();

    TileBatch(const TileBatch&) = delete;
    TileBatch& operator=(const TileBatch&) = delete;

    void buildFromTileMap(const world::TileMap& tileMap, float tileSize);
    void draw() const;
    bool isEmpty() const { return indexCount_ == 0; }

private:
    void uploadBuffers();
    void clearGpu();

    std::vector<TileVertex> vertices_;
    std::vector<GLuint> indices_;
    GLuint vao_ = 0;
    GLuint vbo_ = 0;
    GLuint ebo_ = 0;
    GLsizei indexCount_ = 0;
};

} // namespace render
