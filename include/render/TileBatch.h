#pragma once

#include "render/MeshBuilder.h"
#include "world/TileMap.h"

#include <glad/gl.h>

#include <vector>

namespace render {

class TileBatch {
public:
    TileBatch() = default;
    ~TileBatch();

    TileBatch(const TileBatch&) = delete;
    TileBatch& operator=(const TileBatch&) = delete;

    void buildFromTileMap(const world::TileMap& tileMap, float tileSize);
    void draw() const;
    void destroyGpu();
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
