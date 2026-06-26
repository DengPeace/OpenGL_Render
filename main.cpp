//
//  main.cpp
//  TryGL
//
//  Created by 邓和平 on 2025/8/8.

#include "glframework/core.h"
#include "glframework/shader.h"

#include <iostream>
#include <string>
#include <assert.h> //断言

#include "wrapper/checkError.h"
#include "application/Application.h"

#define STB_IMAGE_IMPLEMENTATION
#include "application/stb_image.h"

#include <unistd.h> // 用于获取当前工作目录
#include "glframework/texture.h"

// 引入相机 + 控制器
#include "application/camera/perspectiveCamera.h"
#include "application/camera/trackBallCameraControl.h"
#include "application/camera/orthographicCamera.h"
#include "application/camera/gameCameraControl.h"

#include "glframework/geometry.h"
#include "glframework/material/phongMaterial.h"
#include "glframework/material/whiteMaterial.h"
#include "glframework/material/depthMaterial.h"
#include "glframework/material/opacityMaskMaterial.h"
#include "glframework/material/screenMaterial.h"
#include "glframework/material/cubeMaterial.h"
#include "glframework/material/phongEnvMaterial.h"
#include "glframework/material/phongInstanceMaterial.h"
#include "glframework/material/grassInstanceMaterial.h"

#include "glframework/mesh/mesh.h"
#include "glframework/mesh/InstancedMesh.h"
#include "glframework/renderer/renderer.h"
#include "glframework/light/pointLight.h"
#include "glframework/light/spotLight.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "glframework/scene.h"
#include "application/assimpLoader.h"
#include "application/assimpInstanceLoader.h"

#include "glframework/framebuffer/framebuffer.h"
#include "glframework/framebuffer/shadowMap.h"

#include "timer.h"

/*
 * 目   标: 实现shadow mapping
   拆分目标:
        1 新建 shadow.vert / shadow.frag 深度pass shader（普通mesh用）
        2 新建 shadowInstance.vert 实例化物体的深度pass shader
        3 新建 ShadowMap FBO 类（深度纹理 + 无颜色附件）
        4 修改 Renderer
            4.1 新增 mShadowShader / mShadowInstanceShader 指针
            4.2 新增 renderShadowMap() — 从光源视角渲染深度到 ShadowMap FBO
            4.3 新增 renderShadowObject() — 递归遍历场景树，用深度shader渲染
            4.4 主渲染 render() 中先执行 shadow pass，再渲染主画面
            4.5 GrassInstance 渲染分支中绑定 shadow map 纹理到 unit 3
        5 修改 grassInstance.vert — 增加 lightVPMatrix uniform，计算顶点灯光空间坐标
        6 修改 grassInstance.frag — 增加 calculateShadow()，4-tap PCF 软阴影
        7 修改 main.cpp — 创建 ShadowMap 对象，计算光源的 VP 矩阵并传入 Renderer
        8 修改 Xcode pbxproj — 加入 shadowMap.cpp 编译源文件
        9 Debug 过程
            9.1 将灯光空间 UV 可视化输出 → 确认 lightVPMatrix 正确
            9.2 将 shadow map 深度可视化输出 → 确认 depth pass 有数据
            9.3 发现 shader 重复定义编译错误 → 修复
            9.4 跳过天空盒（CubeMaterial），排除干扰
            9.5 将光照改为斜向，让阴影更明显
*/

Renderer* renderer = nullptr;
Scene* scene = nullptr;

Framebuffer* framebuffer = nullptr;

unsigned int WIDTH = 800;
unsigned int HEIGHT = 600;

GrassInstanceMaterial* grassMaterial = nullptr;

//Lights 灯光们
DirectionalLight* dirLight = nullptr;

AmbientLight* ambLight = nullptr;

PerspectiveCamera* camera = nullptr;
GameCameraControl* cameraControl = nullptr;

glm::vec3 clearColor{0.9, 0.6, 0.6};



void OnResie(int width, int height) {
    GL_CALL(glViewport(0,0,width, height));
    std::cout << "OnResize" << std::endl;
}

void OnKey(int key, int action, int mods) {
    cameraControl -> onKey(key, action, mods);
}

// 鼠标按下/抬起
void OnMouse(int button, int action, int mods) {
    double x,y;
    glApp -> getCursorPosition(&x, &y);
    cameraControl -> onMouse(button, action, x, y);
}

// 鼠标移动
void OnCursor(double xpos, double ypos) {
    cameraControl -> onCursor(xpos, ypos);
}

// 鼠标滚轮
void OnScroll(double offset){
    cameraControl -> onScroll(offset);
}

void setModelBlend(Object* obj, bool blend, float opacity) {
    if (obj -> getType() == ObjectType::Mesh) { 
        Mesh* mesh = (Mesh*)obj;
        Material* mat = mesh -> mMaterial;
        mat -> mBlend = blend;
        mat -> mOpacity = opacity;
        mat -> mDepthWrite = false;
    }
    auto children = obj -> getChildren();
    for (int i = 0; i < children.size(); i ++) {
        setModelBlend(children[i], blend, opacity);
    }
}

void setInstanceMatrix(Object* obj, int index, glm::mat4 matrix) {
    if (obj->getType() == ObjectType::InstancedMesh) {
        InstancedMesh* im = (InstancedMesh*)obj;
        im->mInstanceMatrices[index] = matrix;
    }

    auto children = obj->getChildren();
    for (int i = 0; i < children.size(); i++) {
        setInstanceMatrix(children[i], index, matrix);
    }
}

void updateInstanceMatrix(Object* obj) {
    if (obj->getType() == ObjectType::InstancedMesh) {
        InstancedMesh* im = (InstancedMesh*)obj;
        im->updateMatrices();
    }

    auto children = obj->getChildren();
    for (int i = 0; i < children.size(); i++) {
        updateInstanceMatrix(children[i]);
    }
}

void setInstanceMaterial(Object* obj, Material* material) {
    if (obj->getType() == ObjectType::InstancedMesh) {
        InstancedMesh* im = (InstancedMesh*)obj;
        im->mMaterial =  material;
    }

    auto children = obj->getChildren();
    for (int i = 0; i < children.size(); i++) {
        setInstanceMaterial(children[i], material);
    }
}

void prepare() {
    // 双缓冲
    glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
    
    renderer = new Renderer();
    scene = new Scene();
    
    
    auto boxGeo = Geometry::createBox(1.0f);
    auto boxMat = new CubeMaterial();
    boxMat->mDiffuse = new Texture("assets/texture/bk.jpg", 0);
    auto boxMesh = new Mesh(boxGeo, boxMat);
    scene -> addChild(boxMesh);
    
//    auto sphereGeo = Geometry::createSphere(4.0f);
//    auto sphereMat = new PhongInstanceMaterial();
//    sphereMat -> mDiffuse = new Texture("assets/texture/earth.png", 0);

    int rNum = 50;
    int cNum = 50;
     
    auto grassModel = AssimpInstanceLoader::load("assets/fbx/grassNew.obj", rNum * cNum);
    
    glm::mat4 translate;
    glm::mat4 rotate;
    glm::mat4 transform;
    
    srand(glfwGetTime());
    for(int r = 0; r < rNum; r ++) {
        for (int c = 0; c < cNum; c ++) {
            translate = glm::translate(glm::mat4(1.0f), glm::vec3(0.2 * r, 0.0f, 0.2 * c));
            rotate = glm::rotate(glm::radians((float)(rand()%90)), glm::vec3(0.0, 1.0, 0.0));
            transform = translate * rotate;
            setInstanceMatrix(grassModel, r * cNum + c, transform);
        }
    }
    updateInstanceMatrix(grassModel);
    
    grassMaterial = new GrassInstanceMaterial();
    grassMaterial -> mDiffuse = new Texture("assets/texture/GRASS.PNG", 0);
    grassMaterial -> mOpacityMask = new Texture("assets/texture/grassMask.png", 1);
    grassMaterial -> mCloudMask = new Texture("assets/texture/CLOUD.PNG", 2);
    grassMaterial -> mBlend = true;
    grassMaterial -> mDepthWrite = false;
    setInstanceMaterial(grassModel, grassMaterial);
    
    scene -> addChild(grassModel);
    
    auto house = AssimpLoader::load("assets/fbx/house.fbx");
    house -> setScale(glm::vec3(0.5f));
    house -> setPosition(glm::vec3(rNum * 0.2f / 2.0f, 0.4, cNum * 0.2f / 2.0f));
    scene -> addChild(house);
    
    
    dirLight = new DirectionalLight();
    dirLight -> mDirection = glm::vec3(-1.0f, -1.0f, -0.5f);  // 与shadow map一致
    dirLight -> mSpecularIntensity = 0.1f;
    
    ambLight = new AmbientLight();
    ambLight -> mColor = glm::vec3(0.2f);
}



void prepareCamera() {
//    透视相机
    camera = new PerspectiveCamera(
        60.0f,
        (float) glApp -> getWidth() / (float) glApp -> getHeigth(),
        0.1f,
        1000.0f
    );
//    正交相机
//    float size = 6.0f;
//    camera = new OrthographicCamera(
//        -size, size, size, -size, size, -size
//    );
    
    cameraControl = new GameCameraControl();
    cameraControl -> setCamera(camera);
    cameraControl -> setSensitivity(0.4f);
    cameraControl -> setSpeed(0.1f);
    
}

void initIMGUI() {
    ImGui::CreateContext(); // 创建imgui上下文
    ImGui::StyleColorsDark();
    
    // 设置ImGui与GLFW和OpenGL的绑定
    ImGui_ImplGlfw_InitForOther(glApp -> getWindow(), true);
    ImGui_ImplOpenGL3_Init("#version 330");
    
}

void renderIMGUI() {
    //1 开启当前的IMGUI渲染
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    
    //2 决定当前的GUI上面有哪些控件，从上到下
    ImGui::Begin("GrassMaterialEditor");
    
    ImGui::Text("GrassColor");
    
    ImGui::SliderFloat("UVScale", &grassMaterial -> mUVScale, 0.0f, 100.0f);
    ImGui::InputFloat("Brightness", &grassMaterial -> mBrightness);
    ImGui::Text("Wind");
    ImGui::InputFloat("windScale", &grassMaterial -> mWindScale);
    ImGui::InputFloat("PhaseScale", &grassMaterial -> mPhaseScale);
    ImGui::ColorEdit3("windDirection", (float*)&grassMaterial -> mWindDirection);
    ImGui::Text("Cloud");
    ImGui::ColorEdit3("CloudWhiteColor", (float*)&grassMaterial -> mCloudWhiteClor);
    ImGui::ColorEdit3("cloudBlackColor", (float*)&grassMaterial -> mCloudBlackClor);
    ImGui::SliderFloat("CloudUVScale", &grassMaterial -> mCloudUVScale, 0.0f, 100.0f);
    ImGui::InputFloat("CloudSpeed", &grassMaterial -> mCloudSpeed);
    ImGui::SliderFloat("CloudLerp", &grassMaterial -> mCloudLerp, 0.0f, 1.0f);
    ImGui::Text("Shadow");
    ImGui::SliderFloat("ShadowIntensity", &grassMaterial -> mShadowIntensity, 0.0f, 1.0f);
    ImGui::Text("Light");
    ImGui::InputFloat("Intensity", &dirLight -> mIntensity);

    ImGui::End();
    
    //3 执行UI渲染
    ImGui::Render();
    //获取当前窗体的宽高
    int display_w, display_h;
    glfwGetFramebufferSize(glApp -> getWindow(), &display_w, &display_h);
    // 重置视口大小
    glViewport(0, 0, display_w, display_h);
    
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    
}


int main() {

    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        std::cout << "当前工作目录: " << cwd << std::endl;
    } else {
        std::cerr << "无法获取工作目录" << std::endl;
    }
    
    if (!glApp -> init(WIDTH, HEIGHT)) {
        return -1;
    }
    
    glApp -> setResizeCallback(OnResie);
    glApp -> setKeyBoardCallback(OnKey);
    glApp -> setMouseCallback(OnMouse);
    glApp -> setCursorCallback(OnCursor);
    glApp -> setScrollCallback(OnScroll);
    
    // 视口（Viewport: 设置窗口中opengl负责渲染的区域
    // 设置opengl视口，以及用什么颜色来清理画布
    GL_CALL(glViewport(0, 0, WIDTH, HEIGHT););
    GL_CALL(glClearColor(0.0f, 0.0f, 0.0f, 1.0f););

    prepareCamera();
    prepare();
    
    // ---- Shadow Map 初始化 ----
    {
        ShadowMap* shadowMap = new ShadowMap(2048, 2048);
        renderer -> setShadowMap(shadowMap);
        
        // 方向光灯光空间的 VP 矩阵
        glm::vec3 center(5.0f, 0.5f, 5.0f);   // 场景中心
        
        // 斜向光照 - 从右上方向左下方照射
        glm::vec3 lightDir = glm::vec3(1.0f, 1.0f, 0.5f);  // 光的方向
        glm::vec3 lightDirN = glm::normalize(lightDir);
        glm::vec3 lightPos = center + lightDirN * 25.0f;    // 光源位置
        
        float size = 15.0f;
        glm::mat4 lightProj = glm::ortho(-size, size, -size, size, 1.0f, 50.0f);
        glm::mat4 lightView = glm::lookAt(lightPos, center, glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 lightVPMatrix = lightProj * lightView;
        
        renderer -> setLightVPMatrix(lightVPMatrix);
    }
    
    initIMGUI();
    
    while(glApp -> update()) {
        Timer a;
        cameraControl -> update();
        renderer -> setClearColor(clearColor);
        renderer -> render(scene, camera,dirLight, ambLight);
        renderIMGUI();
    }
//4 推出程序前做相关清理
    glApp -> destroy();
    
    return 0;
}
