//
//  perspeciveCamera.h
//  TryGL
//
//  Created by 邓和平 on 2025/9/29.
//

#pragma once

#include "camera.h"

class PerspectiveCamera : public Camera {
public:
    PerspectiveCamera(float fovy, float aspect, float near, float far);
    ~PerspectiveCamera();
    
    glm::mat4 getProjectionMatrix() override;
    
    void scale(float deltaScale) override;
    
    void setViewport(int width, int height) {
        mAspectRatio = static_cast<float>(width) / static_cast<float>(height);
        updateProjectionMatrix();
    }
    
    void updateProjectionMatrix() {
        mProjectionMatrix = glm::perspective(
            glm::radians(mFovy),
            mAspectRatio,
            mNear,
            mFar
        );
    }
    
    glm::mat4 getProjectionMatrix() const {
        return mProjectionMatrix;
    }
    
    // 或者如果是正交投影
    void setOrthographicViewport(int width, int height) {
        // 设置正交投影，通常用于全屏四边形
        float halfWidth = width * 0.5f;
        float halfHeight = height * 0.5f;
        mProjectionMatrix = glm::ortho(
            -halfWidth, halfWidth,  // left, right
            -halfHeight, halfHeight, // bottom, top
            -1.0f, 1.0f              // near, far
        );
    }
    
private:
    float mFovy = 0.0f; // 视张角
    float mAspect = 0.0f; // 横纵比
    
    float mAspectRatio = 1.0f;
    glm::mat4 mProjectionMatrix;
};
