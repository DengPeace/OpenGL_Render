//
//  shadowMap.h
//  深度FBO封装 — 只带深度纹理，无颜色附件
//

#pragma once

#include "../core.h"

class ShadowMap {
public:
    ShadowMap(unsigned int width, unsigned int height);
    ~ShadowMap();

    void bind();
    void unbind();

    unsigned int getDepthTexture() const { return mDepthTexture; }
    unsigned int getFBO() const { return mFBO; }
    unsigned int getWidth() const { return mWidth; }
    unsigned int getHeight() const { return mHeight; }

private:
    unsigned int mFBO{ 0 };
    unsigned int mDepthTexture{ 0 };
    unsigned int mWidth{ 1024 };
    unsigned int mHeight{ 1024 };
};
