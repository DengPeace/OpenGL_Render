//
//  cameraControl.cpp
//  TryGL
//
//  Created by 邓和平 on 2025/9/29.
//

#include "cameraControl.h"
#include <iostream>

CameraControl::CameraControl() {
    
}
CameraControl::~CameraControl() {
    
}

// 由于继承CameraControl的子类，有可能会实现自己的逻辑
void CameraControl::onMouse(int button, int action, double xpos, double ypos) {
    //1 判断当前按键是否按下
    bool pressed = action == GLFW_PRESS ? true : false;
    
    //2 如果按下，记录当前按下的位置
    if (pressed) {
        mCurrentX = xpos;
        mCurrentY = ypos;
    }
    
    //3 根据按下的鼠标按键的不同，更改按键位置
    switch (button) {
        case GLFW_MOUSE_BUTTON_LEFT:
            mLeftMouseDown = pressed;
            break;
        case GLFW_MOUSE_BUTTON_RIGHT:
            mRightMouseDown = pressed;
            break;
        case GLFW_MOUSE_BUTTON_MIDDLE:
            mMiddleMouseDown = pressed;
            break;
        default:
            break;
    }
}

void CameraControl::onCursor(double xpos, double ypos) {
    
}

void CameraControl::onKey(int key, int action, int mods) {
    // 过滤掉repeat的情况
    if (action == GLFW_REPEAT) {
        return ;
    }
    
    //1 检测按下或者抬起，给到一个变量
    bool pressed = action == GLFW_PRESS ? true : false;
    
    //2 记录在keyMap
    mKeyMap[key] = pressed;
    
}

// 每一帧渲染之前都要进行调用，每一帧更新的行为可以放在这里
void CameraControl::update() {
    
}

void CameraControl::onScroll(float offset) {
    
}
