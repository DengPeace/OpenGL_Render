//
//  trackBallCameraControl.h
//  TryGL
//
//  Created by 邓和平 on 2025/10/10.
//

#pragma once

#include "cameraControl.h"

class trackBallCameraControl : public CameraControl {
public:
    trackBallCameraControl();
    ~trackBallCameraControl();
    
    //父类中的接口函数，是否需要重写
    void onCursor(double xpos, double ypos) override;
    
    void onScroll(float offset) override;
    
private:
    void pitch(float angle);
    void yaw(float angle);
    
private:
    float mMoveSpeed = 0.005f;
};
