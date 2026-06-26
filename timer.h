#pragma once

#include <iostream>
#include <algorithm>
#include <cmath>        // 建议使用C++标准头文件
#include <memory>
#include <array>
#include <chrono>

class Timer {
public:
    Timer() {
        m_StartTimePoint = std::chrono::high_resolution_clock::now();
    }
    ~Timer() {
        auto endTimePoint = std::chrono::high_resolution_clock::now();
        
        auto start = std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTimePoint).time_since_epoch().count();
        auto end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimePoint).time_since_epoch().count();
        
        auto duration = end - start;
        double ms = duration * 0.001;
        int fps = 1000 / ms;
        
        std::cout << duration << "us (" << ms << "ms)" << "     ";
        std::cout << fps << " fps" << std::endl;
    }
private:
    std::chrono::time_point<std::chrono::high_resolution_clock> m_StartTimePoint;
};
