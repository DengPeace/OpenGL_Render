//
//  phongMaterial.h
//  TryGL
//
//  Created by 邓和平 on 2025/11/21.
//

#pragma once


#include "material.h"
#include "../texture.h"
#include "../core.h"

class OpacityMaskMaterial : public Material {
public:
    OpacityMaskMaterial();
    ~OpacityMaskMaterial();
    
    
public:
    Texture*    mDiffuse { nullptr };
    Texture*    mOpacityMask{ nullptr };
    float       mShiness { 1.0f };
    
    
};
