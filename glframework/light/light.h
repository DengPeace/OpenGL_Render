//
//  light.h
//  TryGL
//
//  Created by 邓和平 on 2025/11/23.
//

#pragma once

#include "../core.h"

class Light {
public:
    Light();
    ~Light();
    
public:
    glm::vec3   mColor {1.0f};
    float       mSpecularIntensity {1.0f};
    float       mIntensity { 1.0f };
};
