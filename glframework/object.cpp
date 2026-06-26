//
//  object.cpp
//  TryGL
//
//  Created by 邓和平 on 2025/11/21.
//
#include "object.h"


Object::Object() {
    mType = ObjectType::Object;
}
Object::~Object() {
    
}

void Object::setPosition(glm::vec3 pos) {
    mPosition = pos;
}


//增量旋转
void Object::rotateX(float angle) {
    mAngleX += angle;
}

void Object::rotateY(float angle) {
    mAngleY += angle;
}

void Object::rotateZ(float angle) {
    mAngleZ += angle;
}

void Object::setAngleX(float angle) {
    mAngleX = angle;
}

void Object::setAngleY(float angle) {
    mAngleY = angle;
}

void Object::setAngleZ(float angle) {
    mAngleZ = angle;
}

void Object::setScale(glm::vec3 scale) {
    mScale = scale;
}

glm::mat4 Object::getModelMatrix() const {
    // 获得parent变换矩阵
    glm::mat4 parentModelMatrix{1.0f};
    if (mParent != nullptr) {
        parentModelMatrix = mParent -> getModelMatrix();
    }
    
    
    // unity : 缩放 旋转 平移
    glm::mat4 transform{ 1.0f };
    
    transform = glm::scale(transform, mScale);
    
    //unity旋转标准，pitch yaw roll
    transform = glm::rotate(transform, glm::radians(mAngleX), glm::vec3(1.0f, 0.0f, 0.0f));
    transform = glm::rotate(transform, glm::radians(mAngleY), glm::vec3(0.0f, 1.0f, 0.0f));
    transform = glm::rotate(transform, glm::radians(mAngleZ), glm::vec3(0.0f, 0.0f, 1.0f));
    
    transform = parentModelMatrix *  glm::translate(glm::mat4(1.0f), mPosition) * transform;
    
    return transform;
}
 

void Object::addChild(Object* obj) {
    //1 检查是否曾经加入过这个孩子
    auto iter = std::find(mChildren.begin(), mChildren.end(), obj);
    if (iter != mChildren.end()) {
        std::cerr << "Duplicated Child added" << std::endl;
        return ;
    }
    
    //2 加入孩子
    mChildren.push_back(obj);
    
    //3 设置孩子的parent
    obj -> mParent = this;
    
}

std::vector<Object*> Object::getChildren() {
    return mChildren;
}

Object* Object::getParent() {
    return mParent;
}
