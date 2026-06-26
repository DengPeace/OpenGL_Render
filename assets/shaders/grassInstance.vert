//
//  vertex.glsl
//  TryGL
//
//  Created by 邓和平 on 2025/9/3.
//


#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aUV;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec3 aColor;
layout (location = 4) in mat4 aInstanceMatrix;

out vec2 uv;
out vec3 normal;
out vec3 worldPosition;
out vec2 worldXZ;
out vec4 vertexPositionLightSpace;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

uniform float time;

uniform float windScale;
uniform vec3 windDirection;
uniform float phaseScale;

uniform mat4 lightVPMatrix;

// aPos做为attribute(属性)传入shader
// 不允许更改 
void main() {
    // 将输入的顶点位置，转换为其次坐标（3维 -> 4维）
    vec4 transformPosition = vec4(aPos, 1.0);
    
    // 做一个中间变量TransformPosition，用于计算四位位置与modelMatrix相乘的中间结果
    transformPosition = modelMatrix * aInstanceMatrix * transformPosition;
    
    // 阴影灯光空间坐标（在风力变形之前计算，与shadow pass匹配）
    vertexPositionLightSpace = lightVPMatrix * modelMatrix * aInstanceMatrix * vec4(aPos, 1.0);
    
    worldXZ = transformPosition.xz;
     
    //风力变动
    vec3 windDirN = normalize(windDirection);
    float phaseDistance = dot(windDirN, transformPosition.xyz);
    transformPosition += vec4(sin(time + phaseDistance / phaseScale) * (1.0 - aColor.r) * windScale * windDirN, 0.0);
    
    // 计算当前顶点的worldPosition，并且向后传输给FragmentShader
    worldPosition = transformPosition.xyz;
    
    
    gl_Position = projectionMatrix * viewMatrix * transformPosition;
    uv = aUV;
    normal = transpose(inverse(mat3(modelMatrix * aInstanceMatrix))) *  aNormal;
//    normal = normalMatrix * aNormal;
}
