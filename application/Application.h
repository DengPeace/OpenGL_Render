//
//  Application.h
//  TryGL
//
//  Created by 邓和平 on 2025/8/19.
//

#pragma once

/*
 * 学习目标: 实现单例类Appliaction的封装
    1. 编写一个单例类的Application
    2. 成员变量 + 成员函数
        2.1 成员函数-init() （初始化）
        2.2 成员函数-update() （每一帧执行）
        2.3 成员函数-destroy() （结尾执行）
    3. 响应回调函数（Resize）
        3.1 声明一个函数指针ResizeCallback
        3.2 声明一个ResieCallback类型的成员变量
        3.3 声明一个SetResizeCallback的函数，设置窗体变化响应回调函数
        3.4 声明一个static的静态函数，用于响应glfw窗体变化
        3.5 将静态函数设置到glfw的监听Resize监听中
        3.6 * 学会使用glfw的UserPointer
    4.键盘响应函数(OnKeyBoard)
        4.1 声明一个static的静态函数，用于响应glfw的键盘事件
        4.2 将静态函数设置到glfw的监听KeyCallback监听当中
        4.3 声明一个函数指针KeyBoardCallback
        4.4 声明一个KeyBoardCallback类型的成员变量
        4.5 声明一个SetKeyBoardCallback的函数，设置键盘响应回调函数
        4.6 * 学会使用glfw的UserPointer
        
 */

#include <iostream>
//#include <glad/glad.h>
//#include <GLFW/glfw3.h>


#define glApp Application::getInstance()

class GLFWwindow;

using ResizeCallback = void(*)(int width, int height);
using KeyBoardCallback = void(*)(int key, int action, int mods);
using MouseCallback = void(*)(int botton, int action, int mods);
using CursorCallback = void(*)(double xpos, double ypos);
using ScrollCallback = void(*)(double offset);

class Application {
public:
    ~Application();
    
    // 用于访问实例的静态函数
    static Application* getInstance();
    
    void test() {
        std::cout << "App test" << std::endl;
    }
    
    bool init(const int& width = 800, const int& height = 600);
    
    bool update();
    
    void destroy();
    
    GLFWwindow* getWindow() const {
        return mWindow;
    }
    
    uint32_t getWidth() const { return mWidth; }
    uint32_t getHeigth() const { return mHeight; }
    void getCursorPosition(double* x, double* y);
    
    void setResizeCallback(ResizeCallback callback) { mResizeCallback = callback; }
    void setKeyBoardCallback(KeyBoardCallback callback) { mKeyBoardCallback = callback; }
    void setMouseCallback(MouseCallback callback) { mMouseCallback = callback; }
    void setCursorCallback(CursorCallback callback) { mCursorCallback = callback; }
    void setScrollCallback(ScrollCallback callback) {mScrollCallback = callback; }
    
private:
    static void frameBufferSizeCallback(GLFWwindow* window, int width, int height);
    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void mouseCallback(GLFWwindow* window, int button, int action, int mods);
    static void cursorCallback(GLFWwindow* window, double xpos, double ypos);
    static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);

    
private:
    
    static Application* mInstance;
    
    uint32_t mWidth{0};
    uint32_t mHeight{0};
    GLFWwindow* mWindow{nullptr};

    ResizeCallback mResizeCallback {nullptr};
    KeyBoardCallback mKeyBoardCallback {nullptr};
    MouseCallback mMouseCallback {nullptr};
    CursorCallback mCursorCallback {nullptr};
    ScrollCallback mScrollCallback {nullptr};
    
    Application();
};
