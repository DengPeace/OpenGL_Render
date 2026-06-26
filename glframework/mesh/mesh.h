//
//  mash.h
//  TryGL
//
//  Created by 邓和平 on 2025/11/23.
//

#pragma once

#include "core.h"
#include "geometry.h"
#include "../material/material.h"
#include "object.h"

class Mesh : public Object {
public:
    Mesh(Geometry* geometry, Material* material);
    ~Mesh();
    
public:
    Geometry* mGeometry {nullptr};
    Material* mMaterial {nullptr};
    
};
