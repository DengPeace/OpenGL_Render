//
//  directionalLight.h
//  TryGL
//
//  Created by 邓和平 on 2025/11/23.
//

#pragma once

#include "../core.h"
#include "light.h"

class DirectionalLight : public Light {
public:
    DirectionalLight();
    ~DirectionalLight();
    
public:
    glm::vec3 mDirection{-1.0f};
     
};
