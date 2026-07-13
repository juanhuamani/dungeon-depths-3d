#include "render/ShadowMap.h"
#include <iostream>

namespace render {

ShadowMap::~ShadowMap() {
    if (fbo_ != 0) glDeleteFramebuffers(1, &fbo_);
    if (depthMap_ != 0) glDeleteTextures(1, &depthMap_);
}

bool ShadowMap::init(unsigned int width, unsigned int height) {
    width_ = width;
    height_ = height;

    glGenFramebuffers(1, &fbo_);
    
    glGenTextures(1, &depthMap_);
    glBindTexture(GL_TEXTURE_2D, depthMap_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width_, height_, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    // Clamp to border to prevent shadows outside the light frustum
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap_, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Error: ShadowMap Framebuffer not complete!" << std::endl;
        return false;
    }
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return true;
}

void ShadowMap::bindForWriting() {
    glViewport(0, 0, width_, height_);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
    glClear(GL_DEPTH_BUFFER_BIT);
}

void ShadowMap::bindForReading(GLenum textureUnit) {
    glActiveTexture(textureUnit);
    glBindTexture(GL_TEXTURE_2D, depthMap_);
}

} // namespace render
