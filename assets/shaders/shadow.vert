//
//  shadow.vert
//  非实例化物体的深度pass
//

#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 modelMatrix;
uniform mat4 lightVPMatrix;

void main() {
    gl_Position = lightVPMatrix * modelMatrix * vec4(aPos, 1.0);
}
