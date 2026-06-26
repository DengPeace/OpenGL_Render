//
//  perspeciveCamera.cpp
//  TryGL
//
//  Created by 邓和平 on 2025/9/29.
//

#include "perspectiveCamera.h"

PerspectiveCamera::PerspectiveCamera(float fovy, float aspect, float near, float far) {
    mFovy = fovy;
    mAspect = aspect;
    mNear = near;
    mFar = far;
}

PerspectiveCamera::~PerspectiveCamera() {
    
}

glm::mat4 PerspectiveCamera::getProjectionMatrix() {
    //注意：传入的是fovy角度，需要的是弧度
    return glm::perspective(glm::radians(mFovy), mAspect, mNear, mFar);
}

void PerspectiveCamera::scale(float deltaScale) {
    // 相机向前的向量
    auto front = glm::cross(mUp, mRight);
    mPosition += (front * deltaScale);
}
