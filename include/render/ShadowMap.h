#pragma once

#include <glad/gl.h>

namespace render {

class ShadowMap {
public:
    ShadowMap() = default;
    ~ShadowMap();

    bool init(unsigned int width, unsigned int height);
    void bindForWriting();
    void bindForReading(GLenum textureUnit);
    
    unsigned int getDepthTexture() const { return depthMap_; }

private:
    unsigned int fbo_ = 0;
    unsigned int depthMap_ = 0;
    unsigned int width_ = 0;
    unsigned int height_ = 0;
};

} // namespace render
