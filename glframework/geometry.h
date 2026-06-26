//
//  geometry.h
//  TryGL
//
//  Created by 邓和平 on 2025/10/15.
//

#pragma once

#include "core.h"

class Geometry {
public:
    Geometry();
    Geometry(
             std::vector<float>& positions,
             std::vector<float>& normals,
             std::vector<float>& uvs,
             std::vector<unsigned int>& indices
    );
    Geometry(
            const std::vector<float>& positions,
            const std::vector<float>& normals,
            const std::vector<float>& uvs,
            const std::vector<float>& colors,
            const std::vector<unsigned int>& indices
        );
    ~Geometry();
    
    static Geometry* createBox(float size);
    static Geometry* createSphere(float radius);
    static Geometry* createPlane(float width, float height);
    static Geometry* createScreenPlane();
    
    GLuint getVao() const { return mVao; }
    uint32_t getIndicesCount() const { return mIndicesCount; }
    
private:
    GLuint mVao {0};
    GLuint mPosVbo {0};
    GLuint mUvVbo {0};
    GLuint mNormalVbo {0};
    GLuint mColorVbo {0};
    GLuint mEbo {0};
    
    uint32_t mIndicesCount {0};
};
