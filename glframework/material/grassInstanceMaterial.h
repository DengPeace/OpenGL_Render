//
//  phongInstanceMaterial.h
//  TryGL
//
//  Created by 邓和平 on 2025/2/20.
//

#pragma once


#include "material.h"
#include "../texture.h"
#include "../core.h"


class GrassInstanceMaterial : public Material {
public:
    GrassInstanceMaterial();
    ~GrassInstanceMaterial();
    
    
public:
    Texture*    mDiffuse { nullptr };
    Texture*    mOpacityMask{ nullptr };
    float       mShiness { 1.0f };
    
    // 草地贴图特性
    float       mUVScale { 7.0f };
    float       mBrightness { 1.9f };
    
    // 风力相关
    float       mWindScale { 0.1f };
    glm::vec3   mWindDirection { 1.0, 1.0, 1.0 };
    float       mPhaseScale { 1.0f };
    
    // 阴影
    float       mShadowIntensity { 0.8f };
    
    // 云朵相关
    Texture*    mCloudMask { nullptr };
    glm::vec3   mCloudWhiteClor{ 0.37, 0.64, 0.45 };
    glm::vec3   mCloudBlackClor{ 0.1, 0.16, 0.18 };
    float       mCloudUVScale { 1.0f };
    float       mCloudSpeed { 0.1f };
    float       mCloudLerp { 0.2f };
};
