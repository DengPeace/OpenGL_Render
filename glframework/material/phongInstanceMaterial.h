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


class PhongInstanceMaterial : public Material {
public:
    PhongInstanceMaterial();
    ~PhongInstanceMaterial();
    
    
public:
    Texture*    mDiffuse { nullptr };
    Texture*    mSpecularMask{ nullptr };
    float       mShiness { 1.0f };
    
    
};
