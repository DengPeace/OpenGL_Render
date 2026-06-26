//
//  cubeMaterial.h
//  TryGL
//
//  Created by 邓和平 on 2026/2/17.
//

#pragma once
#include "material.h"
#include "../texture.h"

class CubeMaterial :public Material {
public:
    CubeMaterial();
    ~CubeMaterial();

public:
    Texture* mDiffuse{ nullptr };
};
