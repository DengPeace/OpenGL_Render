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

class PhongEnvMaterial : public Material {
public:
    PhongEnvMaterial();
    ~PhongEnvMaterial();
    
    
public:
    Texture*    mDiffuse { nullptr };
    Texture*    mSpecularMask{ nullptr };
    Texture*    mEnv { nullptr };
    float       mShiness { 1.0f };
    
    
};
