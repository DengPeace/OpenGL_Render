//
//  material.h
//  TryGL
//
//  Created by 邓和平 on 2025/11/21.
//

#pragma once
#include "../core.h"

//使用c++的枚举
enum class MaterialType {
    PhongMaterial,
    WhiteMaterial,
    DepthMaterial,
    OpacityMaskMaterial,
    ScreenMaterial,
    CubeMaterial,
    PhongEnvMaterial,
    PhongInstanceMaterial,
    GrassInstanceMaterial
};


class Material {
public:
    Material();
    ~Material();
    
public:
    MaterialType mType;
    
    // 深度检测相关
    bool    mDepthTest{ true };
    GLenum  mDepthFunc{ GL_LEQUAL };
    bool    mDepthWrite { true };
    
    //polygonOffset 相关
    bool            mPolygonOffset{ false };
    unsigned int    mPolygonOffsetType { GL_POLYGON_OFFSET_FILL };
    float           mFactor{ 0.0f };
    float           mUnit{ 0.0f };
    
    //stencil相关
    bool            mStencilTest { false };
    
    unsigned int    mSFail { GL_KEEP };     // 模板测试失败了怎么办
    unsigned int    mZFail { GL_KEEP };     // 模板测试通过了到那时深度测试没有通过怎么办
    unsigned int    mZPass { GL_KEEP };     // 模板 + 深度测试都通过了怎么办
    
    unsigned int    mStencilMask { 0xFF };
    
    unsigned int    mStencilFunc { GL_ALWAYS };
    unsigned int    mStencilRef  { 0 };
    unsigned int    mStencilFuncMask { 0xFF };
    
    // 颜色混合相关
    bool            mBlend{ false };
    unsigned int    mSFactor{ GL_SRC_ALPHA };
    unsigned int    mDFactor{ GL_ONE_MINUS_SRC_ALPHA };
    
    float           mOpacity{ 1.0f };
    
    // Face Culling
    bool            mFaceCulling {false};
    unsigned int    mFrontFace { GL_CCW };
    unsigned int    mCullFace { GL_BACK };
    
};
