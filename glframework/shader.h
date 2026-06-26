//
//  shader.h
//  TryGL
//
//  Created by 邓和平 on 2025/9/3.
//

#pragma once

#include "core.h"
#include <string>

class Shader{
public:
    Shader(const char* vertexPath, const char* fragmentPath);
    ~Shader();
    
    void begin(); // 开始使用当前的Shader
    
    void end(); // 结束使用当前的Shader
    
    void setFloat(const std::string& name, float value);
    
    void setVector3(const std::string& name, float x, float y, float z);
    void setVector3(const std::string& name, const float* values);
    void setVector3(const std::string& name, const glm::vec3 value);
    
    void setInt(const std::string& name, int value);
    
    void setMatrix4x4(const std::string& name, glm::mat4 value);
    void setMatrix4x4Array(const std::string& name, glm::mat4* value, int count);
    void setMatrix3x3(const std::string& name, glm::mat3 value);
    
private:
    //shader program
    // type: COMPILE LINK
    void checkShaderErrors(GLuint tatget, std::string type);
    
private:
    GLuint mProgram{0};
};
