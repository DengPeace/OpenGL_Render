//
//  shadowInstance.vert
//  实例化物体的深度pass（处理 aInstanceMatrix）
//

#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 4) in mat4 aInstanceMatrix;

uniform mat4 modelMatrix;
uniform mat4 lightVPMatrix;

void main() {
    gl_Position = lightVPMatrix * modelMatrix * aInstanceMatrix * vec4(aPos, 1.0);
}
