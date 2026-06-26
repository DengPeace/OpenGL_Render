//
//  renderer.h
//  TryGL
//
//  Created by 邓和平 on 2026/1/8.
//
// #ifndef __CORE_H__
// #define __CORE_H__
// #endif __core_h__

#pragma once

#include <vector>
#include "../core.h"
#include "../mesh/mesh.h"
#include "../../application/camera/camera.h"
#include "../../application/camera/perspectiveCamera.h"
#include "../light/directionalLight.h"
#include "../light/pointLight.h"
#include "../light/spotLight.h"
#include "../light/ambientLight.h"
#include "../shader.h"
#include "../scene.h"
#include "../framebuffer/shadowMap.h"


class Renderer {
public :
    Renderer();
    ~Renderer();
    
    //渲染功能函数：
    // 每次调用都会渲染一帧
    
    void render(
        Scene* scene,
        PerspectiveCamera* camera,
        DirectionalLight* dirLight,
        AmbientLight* ambLight,
        unsigned int fbo = 0
    );
    
    void renderObject(
        Object* object,
        PerspectiveCamera* camera,
        DirectionalLight* dirLight,
        AmbientLight* ambLight
    );
    
    
//    void render(
//        const std::vector<Mesh*>& meshes,
//        Camera* camera,
//        DirectionalLight* dirLight,
//        const std::vector<PointLight*>& pointLights,
//        SpotLight* SpotLight,
//        AmbientLight* ambLight
//    );
    
//    render(
//       const std::vector<Mesh*>& meshes,
//       Camera* camera,
//       PointLight* pointLight,
//       AmbientLight* ambLight
//   );
    
//    void render(
//        const std::vector<Mesh*>& meshes,
//        Camera* camera,
//        DirectionalLight* dirLight,
//        AmbientLight* ambLight
//    );
    
    void setClearColor(glm::vec3 color);
    
    // Shadow Map
    void setShadowMap(ShadowMap* sm) { mShadowMap = sm; }
    void setLightVPMatrix(glm::mat4 matrix) { mLightVPMatrix = matrix; }
    
public:
    Material* mGlobalMaterial {nullptr};
    
private:
    void projectObject(Object* obj);
    
    // Shadow Map 渲染
    void renderShadowMap(Scene* scene);
    void renderShadowObject(Object* object, const glm::mat4& lightVPMatrix);
    
    // 根据Material类型不同，挑选不同的shader
    Shader* pickShader(MaterialType type);
    
    void setDepthState(Material* material);
    void setPolygonOffsetState(Material* material);
    void setStencilState(Material* material);
    void setBlendState(Material* material);
    void setFaceCullingState(Material* material);
    
    
private:
    //生成多种不同的shader对象
    //根据材质类型不同挑选使用那一个shader对象
    Shader* mPhongShader { nullptr };
    Shader* mWhiteShader { nullptr };
    Shader* mDepthShader { nullptr };
    Shader* mOpacityMaskShader { nullptr };
    Shader* mScreenShader{ nullptr };
    Shader* mCubeShader { nullptr };
    Shader* mPhongEnvShader { nullptr };
    Shader* mPhongInstanceShader { nullptr };
    Shader* mGrassInstanceShader { nullptr };
    
    // Shadow Map shaders
    Shader* mShadowShader { nullptr };
    Shader* mShadowInstanceShader { nullptr };
    
    // Shadow Map 状态
    ShadowMap* mShadowMap { nullptr };
    glm::mat4 mLightVPMatrix{ 1.0f };
    
    glm::vec3 mLastSortCamPos{ 0.0f };
    
    //不透明物体与透明物体的队列
    //注意！！ *****每一帧绘制前，需要清空两个队列*****
    std::vector<Mesh*> mOpacityObjects{};
    std::vector<Mesh*> mTransparentObjects{};
    
};
