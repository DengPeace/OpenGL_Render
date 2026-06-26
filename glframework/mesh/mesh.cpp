//
//  mash.cpp
//  TryGL
//
//  Created by 邓和平 on 2025/11/23.
//

#include "mesh.h"

Mesh::Mesh(Geometry* geometry, Material* material) {
    mGeometry = geometry;
    mMaterial = material;
    mType = ObjectType::Mesh;
}

Mesh::~Mesh() {
    delete mGeometry;
    delete mMaterial;
}
