//
//  renderer.cpp
//  TryGL
//
//  Created by 邓和平 on 2026/1/8.
//

#include "renderer.h"
#include <iostream>
#include "../../application/Application.h"
#include "../material/phongMaterial.h"
#include "../material/whiteMaterial.h"
#include "../material/opacityMaskMaterial.h"
#include "../material/screenMaterial.h"
#include "../material/cubeMaterial.h"
#include "../material/phongEnvMaterial.h"
#include "../material/phongInstanceMaterial.h"
#include "../material/grassInstanceMaterial.h"
#include "../../application/camera/perspectiveCamera.h"
#include "../mesh/instancedMesh.h"
#include <string>
#include <algorithm>

Renderer::Renderer() {
    mPhongShader = new Shader("assets/shaders/phong.vert", "assets/shaders/phong.frag");
    mWhiteShader = new Shader("assets/shaders/white.vert", "assets/shaders/white.frag");
    mDepthShader = new Shader("assets/shaders/depth.vert", "assets/shaders/depth.frag");
    mOpacityMaskShader = new Shader("assets/shaders/phongOpacityMask.vert", "assets/shaders/phongOpacityMask.frag");
    mScreenShader = new Shader("assets/shaders/screen.vert", "assets/shaders/screen.frag");
    mCubeShader = new Shader("assets/shaders/cube.vert", "assets/shaders/cube.frag");
    mPhongEnvShader = new Shader("assets/shaders/phongEnv.vert", "assets/shaders/phongEnv.frag");
    mPhongInstanceShader = new Shader("assets/shaders/phongInstance.vert", "assets/shaders/phongInstance.frag");
    mGrassInstanceShader = new Shader("assets/shaders/grassInstance.vert", "assets/shaders/grassInstance.frag");
    
    mShadowShader = new Shader("assets/shaders/shadow.vert", "assets/shaders/shadow.frag");
    mShadowInstanceShader = new Shader("assets/shaders/shadowInstance.vert", "assets/shaders/shadow.frag");
    
}
Renderer::~Renderer() {
    
}

void Renderer::setClearColor(glm::vec3 color) {
    glClearColor(color.r, color.g, color.b, 1.0);
}

void Renderer::render(
    Scene* scene,
    PerspectiveCamera* camera,
    DirectionalLight* dirLight,
    AmbientLight* ambLight,
    unsigned int fbo
) {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    
    /*-----------------------------------------自己加的------------*/
//    // 根据渲染目标设置视口
//    int viewportWidth, viewportHeight;
//    if(fbo != 0) {
//        // 离屏渲染：使用FBO的尺寸
//        viewportWidth = 1600;
//        viewportHeight = 1200;
//        glViewport(0, 0, viewportWidth, viewportHeight);
//    } else {
//        // 屏幕渲染：使用窗口尺寸
//        viewportWidth = glApp -> getWidth();  // 宽度
//        viewportHeight = glApp -> getHeigth(); // 高度
//        
//        // 设置当前视口
//        glViewport(0, 0, viewportWidth, viewportHeight);
//    }
//    
    // 关键：更新相机的投影矩阵以匹配当前视口
//    camera->setViewport(viewportWidth, viewportHeight);
    /*-------------------------------------------------------*/
    
    //1 设置当前帧绘制时，OpenGL的必要状态级参数
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);
    
    glDisable(GL_POLYGON_OFFSET_FILL);
    glDisable(GL_POLYGON_OFFSET_LINE);
    
    //开启测试，设置基本写入状态，打开模版测试
    glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    glStencilMask(0xFF); // 保证了模板缓冲可以被清理
    
    //默认颜色混合
    glDisable(GL_BLEND);
    
    //2 清理画布
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    
    // ---- Shadow Map Pass ----
    if (mShadowMap) {
        int mainVP[4];
        glGetIntegerv(GL_VIEWPORT, mainVP);
        
        renderShadowMap(scene);
        
        glViewport(mainVP[0], mainVP[1], mainVP[2], mainVP[3]);
    }
    
    // 清空两个队列
    mOpacityObjects.clear();
    mTransparentObjects.clear();
    
    projectObject(scene);
    
    // 只在相机移动时排序（避免每帧 O(n log n) 开销）
    glm::vec3 currentCamPos = camera -> mPosition;
    
    if (glm::length(currentCamPos - mLastSortCamPos) > 0.001f || mTransparentObjects.empty()) {
        mLastSortCamPos = currentCamPos;
        glm::mat4 viewMatrix = camera -> getViewMatrix();
        
        std::sort(
            mTransparentObjects.begin(),
            mTransparentObjects.end(),
            [&viewMatrix](const Mesh* a, const Mesh* b) {
                auto wa = a -> getModelMatrix() * glm::vec4(0.0, 0.0, 0.0, 1.0);
                auto wb = b -> getModelMatrix() * glm::vec4(0.0, 0.0, 0.0, 1.0);
                auto za = (viewMatrix * wa).z;
                auto zb = (viewMatrix * wb).z;
                return za < zb;
            }
        );
    }
    
    
    //3 渲染两个队列
    for(int i = 0; i < mOpacityObjects.size(); i ++) {
        renderObject(mOpacityObjects[i], camera, dirLight, ambLight);
    }
    for(int i  = 0; i < mTransparentObjects.size(); i ++) {
        renderObject(mTransparentObjects[i], camera, dirLight, ambLight);
    }
}


void Renderer::projectObject(Object* obj) {
    if (obj -> getType() == ObjectType:: Mesh || obj -> getType() == ObjectType::InstancedMesh) {
        Mesh* mesh = (Mesh*)obj;
        auto material = mesh -> mMaterial;
        if (material -> mBlend) {
            mTransparentObjects.push_back(mesh);
        } else {
            mOpacityObjects.push_back(mesh);
        }
    }
    auto children= obj -> getChildren();
    for (int i = 0; i < children.size(); i++) {
        projectObject(children[i]);
    }
}


Shader* Renderer::pickShader(MaterialType type) {
    Shader* result = nullptr;
    
    switch(type) {
        case MaterialType::PhongMaterial:
            result = mPhongShader;
            break;
        case MaterialType::WhiteMaterial:
            result = mWhiteShader;
            break;
        case MaterialType::DepthMaterial:
            result = mDepthShader;
            break;
        case MaterialType::OpacityMaskMaterial:
            result = mOpacityMaskShader;
            break;
        case MaterialType::ScreenMaterial:
            result = mScreenShader;
            break;
        case MaterialType::CubeMaterial:
            result = mCubeShader;
            break;
        case MaterialType::PhongEnvMaterial:
            result = mPhongEnvShader;
            break;
        case MaterialType::PhongInstanceMaterial:
            result = mPhongInstanceShader;
            break;
        case MaterialType::GrassInstanceMaterial:
            result = mGrassInstanceShader;
            break;
        default:
            std::cout << "Unknown material type to pick shader" << std::endl;
            break;
    }
    
    return result;
}


//针对单个object进行渲染
void Renderer::renderObject(
    Object* object,
    PerspectiveCamera* camera,
    DirectionalLight* dirLight,
    AmbientLight* ambLight
) {
    //1 判断是Mesh还是Object，如果是Mesh需要渲染
    if(object -> getType() == ObjectType::Mesh || object -> getType() == ObjectType::InstancedMesh) {
        auto mesh = (Mesh*)object;
        auto geometry = mesh -> mGeometry;
        
        Material* material = nullptr;
        if(mGlobalMaterial != nullptr) {
            material = mGlobalMaterial;
        } else {
            material = mesh -> mMaterial;
        }
        
        // 设置渲染状态
        setDepthState(material);
        setPolygonOffsetState(material);
        setStencilState(material);
        setBlendState(material);
        setFaceCullingState(material);
        
        //1 决定使用那个Shader
        Shader * shader = pickShader(material -> mType);

        //2 更新shader的uniform
        shader -> begin();

        switch (material -> mType) {
            case MaterialType::PhongMaterial: {
                PhongMaterial* phongMat = (PhongMaterial*)material;

                //diffuse贴图帧更新
                //将纹理采样器与纹理单元挂钩
                shader -> setInt("sampler", 0);

                //将纹理与纹理单元进行挂钩
                phongMat -> mDiffuse -> bind();

                //高光蒙版的帧更新
//                shader -> setInt("specularMaskSampler", 1);
//                phongMat -> mSpecularMask -> bind();


                //mvp
                shader -> setMatrix4x4("modelMatrix", mesh -> getModelMatrix());
                shader -> setMatrix4x4("viewMatrix", camera -> getViewMatrix());
                shader -> setMatrix4x4("projectionMatrix", camera -> getProjectionMatrix());
                
                //计算NormalMatrix
                glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(mesh -> getModelMatrix())));
                shader -> setMatrix3x3("normalMatrix", normalMatrix);

                // 光源参数的uniform更新
                // directionalLight的更新
                shader -> setVector3("directionalLight.color", dirLight -> mColor);
                shader -> setVector3("directionalLight.direction", dirLight -> mDirection);
                shader -> setFloat("directionalLight.specularIntensity", dirLight -> mSpecularIntensity);
                shader -> setFloat("directionalLight.intensity", dirLight -> mIntensity);

                shader -> setFloat("shiness", phongMat -> mShiness);

                shader -> setVector3("ambientColor",  ambLight -> mColor);

                // 相机信息更新
                shader -> setVector3("cameraPosition", camera -> mPosition);
                
                //透明度
                shader -> setFloat("opacity", material -> mOpacity);
                
            }
                break;
            case MaterialType::OpacityMaskMaterial: {
                OpacityMaskMaterial* opacityMat = (OpacityMaskMaterial*)material;


                //diffuse贴图帧更新
                //将纹理采样器与纹理单元挂钩
                shader -> setInt("sampler", 0);

                //将纹理与纹理单元进行挂钩
                opacityMat -> mDiffuse -> bind();

                //opacityMask的帧更新
                shader -> setInt("opacityMaskSampler", 1);
                opacityMat -> mOpacityMask -> bind();


                //mvp
                shader -> setMatrix4x4("modelMatrix", mesh -> getModelMatrix());
                shader -> setMatrix4x4("viewMatrix", camera -> getViewMatrix());
                shader -> setMatrix4x4("projectionMatrix", camera -> getProjectionMatrix());
                
                //计算NormalMatrix
                glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(mesh -> getModelMatrix())));
                shader -> setMatrix3x3("normalMatrix", normalMatrix);

                // 光源参数的uniform更新
                // directionalLight的更新
                shader -> setVector3("directionalLight.color", dirLight -> mColor);
                shader -> setVector3("directionalLight.direction", dirLight -> mDirection);
                shader -> setFloat("directionalLight.specularIntensity", dirLight -> mSpecularIntensity);

                shader -> setFloat("shiness", opacityMat -> mShiness);

                shader -> setVector3("ambientColor",  ambLight -> mColor);

                // 相机信息更新
                shader -> setVector3("cameraPosition", camera -> mPosition);
                
                //透明度
                shader -> setFloat("opacity", material -> mOpacity);
            }
                break;
            case MaterialType::DepthMaterial: {
                //mvp
                shader -> setMatrix4x4("modelMatrix", mesh -> getModelMatrix());
                shader -> setMatrix4x4("viewMatrix", camera -> getViewMatrix());
                shader -> setMatrix4x4("projectionMatrix", camera -> getProjectionMatrix());
                
                shader -> setFloat("near", camera -> mNear);
                shader -> setFloat("far", camera -> mFar);
            }
                break;
            case MaterialType::WhiteMaterial: {
                //mvp
                shader -> setMatrix4x4("modelMatrix", mesh -> getModelMatrix());
                shader -> setMatrix4x4("viewMatrix", camera -> getViewMatrix());
                shader -> setMatrix4x4("projectionMatrix", camera -> getProjectionMatrix());
            }
                break;
            case MaterialType::ScreenMaterial: {
                ScreenMaterial* screenMat = (ScreenMaterial*)material;
                shader->setInt("screenTexSampler", 0);
                screenMat->mScreenTexture->bind();
            }
                 break;
            case MaterialType::CubeMaterial: {
                CubeMaterial* cubeMat = (CubeMaterial*)material;
                mesh -> setPosition(camera -> mPosition);
                //mvp
                shader -> setMatrix4x4("modelMatrix", mesh -> getModelMatrix());
                shader -> setMatrix4x4("viewMatrix", camera -> getViewMatrix());
                shader -> setMatrix4x4("projectionMatrix", camera -> getProjectionMatrix());
                
                shader->setInt("cubeSampler", 0);
                cubeMat->mDiffuse->bind();
            }
                break;
            case MaterialType::PhongEnvMaterial: {
                PhongEnvMaterial* phongMat = (PhongEnvMaterial*)material;

                shader -> setInt("sampler", 0);
                phongMat -> mDiffuse -> bind();

                shader -> setInt("envSampler", 1);
                phongMat -> mEnv -> bind();

                //mvp
                shader -> setMatrix4x4("modelMatrix", mesh -> getModelMatrix());
                shader -> setMatrix4x4("viewMatrix", camera -> getViewMatrix());
                shader -> setMatrix4x4("projectionMatrix", camera -> getProjectionMatrix());
                
                //计算NormalMatrix
                glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(mesh -> getModelMatrix())));
                shader -> setMatrix3x3("normalMatrix", normalMatrix);

                // 光源参数的uniform更新
                // directionalLight的更新
                shader -> setVector3("directionalLight.color", dirLight -> mColor);
                shader -> setVector3("directionalLight.direction", dirLight -> mDirection);
                shader -> setFloat("directionalLight.specularIntensity", dirLight -> mSpecularIntensity);

                shader -> setFloat("shiness", phongMat -> mShiness);

                shader -> setVector3("ambientColor",  ambLight -> mColor);

                // 相机信息更新
                shader -> setVector3("cameraPosition", camera -> mPosition);
                
                //透明度
                shader -> setFloat("opacity", material -> mOpacity);
            }
                break;
            case MaterialType::PhongInstanceMaterial: {
                PhongInstanceMaterial* phongMat = (PhongInstanceMaterial*)material;
                InstancedMesh* im = (InstancedMesh*)mesh;

                //diffuse贴图帧更新
                //将纹理采样器与纹理单元挂钩
                shader -> setInt("sampler", 0);

                //将纹理与纹理单元进行挂钩
                phongMat -> mDiffuse -> bind();

                //mvp
                shader -> setMatrix4x4("modelMatrix", mesh -> getModelMatrix());
                shader -> setMatrix4x4("viewMatrix", camera -> getViewMatrix());
                shader -> setMatrix4x4("projectionMatrix", camera -> getProjectionMatrix());
                
                //计算NormalMatrix
//                glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(mesh -> getModelMatrix())));
//                shader -> setMatrix3x3("normalMatrix", normalMatrix);

                // 光源参数的uniform更新
                // directionalLight的更新
                shader -> setVector3("directionalLight.color", dirLight -> mColor);
                shader -> setVector3("directionalLight.direction", dirLight -> mDirection);
                shader -> setFloat("directionalLight.specularIntensity", dirLight -> mSpecularIntensity);

                shader -> setFloat("shiness", phongMat -> mShiness);

                shader -> setVector3("ambientColor",  ambLight -> mColor);

                // 相机信息更新
                shader -> setVector3("cameraPosition", camera -> mPosition);
                
                //透明度
                shader -> setFloat("opacity", material -> mOpacity);
                
            }
                break;
            case MaterialType::GrassInstanceMaterial: {
                GrassInstanceMaterial* grassMat = (GrassInstanceMaterial*)material;
                InstancedMesh* im = (InstancedMesh*)mesh;
                
                im -> sortMatrices(camera -> getViewMatrix());
                im -> updateMatrices();

                shader -> setInt("sampler", 0);
                grassMat -> mDiffuse -> bind();
                
                shader -> setInt("opacityMask", 1);
                grassMat -> mOpacityMask -> bind();

                //mvp
                shader -> setMatrix4x4("modelMatrix", mesh -> getModelMatrix());
                shader -> setMatrix4x4("viewMatrix", camera -> getViewMatrix());
                shader -> setMatrix4x4("projectionMatrix", camera -> getProjectionMatrix());
                
                //计算NormalMatrix
                glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(mesh -> getModelMatrix())));
                shader -> setMatrix3x3("normalMatrix", normalMatrix);

                // 光源参数的uniform更新
                // directionalLight的更新
                shader -> setVector3("directionalLight.color", dirLight -> mColor);
                shader -> setVector3("directionalLight.direction", dirLight -> mDirection);
                shader -> setFloat("directionalLight.specularIntensity", dirLight -> mSpecularIntensity);

                shader -> setFloat("shiness", grassMat -> mShiness);

                shader -> setVector3("ambientColor",  ambLight -> mColor);

                // 相机信息更新
                shader -> setVector3("cameraPosition", camera -> mPosition);
                
                //透明度
                shader -> setFloat("opacity", material -> mOpacity);
                
                // 贴图特性
                shader -> setFloat("uvScale", grassMat -> mUVScale);
                shader -> setFloat("brightness", grassMat -> mBrightness);
                shader -> setFloat("time", glfwGetTime());
                
                // 风力
                shader -> setFloat("windScale", grassMat -> mWindScale);
                shader -> setFloat("phaseScale", grassMat -> mPhaseScale);
                shader -> setVector3("windDirection", grassMat -> mWindDirection);
                
                // 云层
                shader -> setInt("cloudMask", 2);
                grassMat -> mCloudMask -> bind();
                shader -> setVector3("cloudWhiteColor", grassMat -> mCloudWhiteClor);
                shader -> setVector3("cloudBlackColor", grassMat -> mCloudBlackClor);
                shader -> setFloat("cloudUVScale", grassMat -> mCloudUVScale);
                shader -> setFloat("cloudSpeed", grassMat -> mCloudSpeed);
                shader -> setFloat("cloudLerp", grassMat -> mCloudLerp);
                
                // Shadow Map
                if (mShadowMap) {
                    shader -> setInt("shadowMap", 3);
                    glActiveTexture(GL_TEXTURE3);
                    glBindTexture(GL_TEXTURE_2D, mShadowMap -> getDepthTexture());
                }
                shader -> setMatrix4x4("lightVPMatrix", mLightVPMatrix);
                shader -> setFloat("shadowIntensity", grassMat -> mShadowIntensity);
                
            }
                break;
            default:
                break;
        }

        //3 绑定vao
        glBindVertexArray(geometry -> getVao());

        //4 执行绘制命令
        if(object -> getType() == ObjectType::InstancedMesh) {
            InstancedMesh* im = (InstancedMesh*)mesh;
            glDrawElementsInstanced(GL_TRIANGLES, geometry -> getIndicesCount(), GL_UNSIGNED_INT, 0, im -> mInstanceCount);

        } else {
            glDrawElements(GL_TRIANGLES, geometry -> getIndicesCount(), GL_UNSIGNED_INT, 0);
        }
    }
}

void Renderer::setDepthState(Material* material) {
    if (material -> mDepthTest) {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(material -> mDepthFunc);
    } else {
        glDisable(GL_DEPTH_TEST);
    }
    
    if (material -> mDepthWrite) {
        glDepthMask(GL_TRUE);
    } else {
        glDepthMask(GL_FALSE);
    }
}

void Renderer::setPolygonOffsetState(Material* material) {
    if (material -> mPolygonOffset) {
        glEnable(material -> mPolygonOffsetType);
        glPolygonOffset(material -> mFactor, material -> mUnit);
    } else {
        glDisable(GL_POLYGON_OFFSET_FILL);
        glDisable(GL_POLYGON_OFFSET_LINE);
    }
}

void Renderer::setStencilState(Material* material) {
    if (material -> mStencilTest) {
        glEnable(GL_STENCIL_TEST);

        glStencilOp(material -> mSFail, material -> mZFail, material -> mZPass);
        glStencilMask(material -> mStencilMask);
        glStencilFunc(material -> mStencilFunc, material -> mStencilRef, material -> mStencilFuncMask);
        
        
    } else {
        glDisable(GL_STENCIL_TEST);
    }
    
}

void Renderer::setBlendState(Material* material) {
    if (material -> mBlend) {
        glEnable(GL_BLEND);
        glBlendFunc(material -> mSFactor, material -> mDFactor);
    } else {
        glDisable(GL_BLEND);
    }
}


void Renderer::setFaceCullingState(Material* material) {
    if (material -> mFaceCulling) {
        glEnable(GL_CULL_FACE);
        glFrontFace(material -> mFrontFace);
        glCullFace(material -> mCullFace);
    } else {
        glDisable(GL_CULL_FACE);
    }
}


// ============ Shadow Map ============

void Renderer::renderShadowMap(Scene* scene) {
    // 1. 绑定 Shadow Map FBO
    mShadowMap -> bind();
    
    // 2. 设置深度测试（不需要颜色输出，不需要混合/模板等）
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
    glDisable(GL_STENCIL_TEST);
    glDisable(GL_CULL_FACE);
    
    // 3. 递归渲染所有物体
    renderShadowObject(scene, mLightVPMatrix);
    
    // 4. 解绑
    mShadowMap -> unbind();
}

void Renderer::renderShadowObject(Object* object, const glm::mat4& lightVPMatrix) {
    // 只渲染普通 Mesh
    if (object -> getType() == ObjectType::Mesh) {
        Mesh* mesh = (Mesh*)object;
        
        // 跳过天空盒（CubeMaterial）
        if (mesh -> mMaterial && mesh -> mMaterial -> mType == MaterialType::CubeMaterial) {
            auto children = object -> getChildren();
            for (int i = 0; i < children.size(); i++) {
                renderShadowObject(children[i], lightVPMatrix);
            }
            return;
        }
        
        Geometry* geometry = mesh -> mGeometry;
        
        glm::mat4 modelMat = mesh -> getModelMatrix();
        glm::vec3 pos = modelMat * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
//        std::cout << "Shadow render mesh at world pos: " << pos.x << ", " << pos.y << ", " << pos.z << std::endl;
        
        mShadowShader -> begin();
        mShadowShader -> setMatrix4x4("modelMatrix", modelMat);
        mShadowShader -> setMatrix4x4("lightVPMatrix", lightVPMatrix);
        
        glBindVertexArray(geometry -> getVao());
        glDrawElements(GL_TRIANGLES, geometry -> getIndicesCount(), GL_UNSIGNED_INT, 0);
    }
    
    // 递归子物体
    auto children = object -> getChildren();
    for (int i = 0; i < children.size(); i++) {
        renderShadowObject(children[i], lightVPMatrix);
    }
}

