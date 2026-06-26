//
//  trackBallCameraControl.cpp
//  TryGL
//
//  Created by 邓和平 on 2025/10/10.
//

#include "trackBallCameraControl.h"

trackBallCameraControl::trackBallCameraControl() {
    
}
trackBallCameraControl::~trackBallCameraControl() {
    
}

void trackBallCameraControl::onCursor(double xpos, double ypos) {
    if (mLeftMouseDown) {
        // 调整相机的各类参数
        //1 计算经线和纬线的增量角度（正负都有可能）
        float deltaX = (xpos - mCurrentX) * mSensitivity;
        float deltaY = (ypos - mCurrentY) * mSensitivity;
        
        //2 分开pitch跟yaw各自计算
        pitch(-deltaY);
        yaw(-deltaX);
    } else if (mMiddleMouseDown) {
        float deltaX = (xpos - mCurrentX) * mMoveSpeed;
        float deltaY = (ypos - mCurrentY) * mMoveSpeed;
        
        mCamera -> mPosition += mCamera -> mUp * deltaY;
        mCamera -> mPosition -= mCamera -> mRight * deltaX;
        
    }
    
    mCurrentX = xpos;
    mCurrentY = ypos;
}

void trackBallCameraControl::pitch(float angle) {
    // 绕着mRight向量在旋转
    auto mat = glm::rotate(glm::mat4(1.0f), glm::radians(angle), mCamera -> mRight);
    
    // 影响当前相机的up向量和位置
    // vec4最后一个齐次数0.0f表示向量，1.0f表示一个点
    mCamera -> mUp = mat * glm::vec4(mCamera -> mUp, 0.0f); // vec4给到vec3，给了xyz
    mCamera -> mPosition = mat * glm::vec4(mCamera -> mPosition, 1.0f);
}

void trackBallCameraControl::yaw(float angle) {
    // 绕着世界坐标系的y轴旋转
    auto mat = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
    
    mCamera -> mUp = mat * glm::vec4(mCamera -> mUp, 0.0f);
    mCamera -> mRight = mat * glm::vec4(mCamera -> mRight, 0.0f);
    mCamera -> mPosition = mat * glm::vec4(mCamera -> mPosition, 1.0f);
}

void trackBallCameraControl::onScroll(float offset) {
    mCamera -> scale(mScaleSpeed * offset);
}
