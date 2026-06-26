//
//  fragment.glsl
//  TryGL
//
//  Created by 邓和平 on 2025/9/3.
//

#version 330 core
out vec4 FragColor;

in vec2 uv;
in vec3 normal;
in vec3 worldPosition;
in vec2 worldXZ;
in vec4 vertexPositionLightSpace;

uniform sampler2D sampler; // diffuse贴图采样器
uniform sampler2D opacityMask; // 透明蒙版
uniform sampler2D cloudMask;
uniform sampler2D shadowMap;
uniform float shadowIntensity;
 
uniform vec3 ambientColor;

// 相机世界位置
uniform vec3 cameraPosition;

uniform float shiness;

//透明度
uniform float opacity;

// 草地贴图特性
uniform float uvScale;
uniform float brightness;

uniform vec3 cloudWhiteColor;
uniform vec3 cloudBlackColor;
uniform float cloudUVScale;
uniform float cloudSpeed;
uniform float cloudLerp;

uniform float time;
uniform vec3 windDirection;


struct DirectionalLight {
    vec3 direction;
    vec3 color;
    float specularIntensity;
};

struct PointLight {
    vec3 position;
    vec3 color;
    float specularIntensity;
    
    float k2;
    float k1;
    float kc;
};

struct SpotLight {
    vec3 position;
    vec3 targetDirection;
    vec3 color;
    float outerLine;
    float innerLine;
    float specularIntensity;
};

uniform SpotLight spotLight;
uniform DirectionalLight directionalLight;

#define POINT_LIGHT_NUM 4
uniform PointLight pointLights[POINT_LIGHT_NUM];

//计算漫反射光照
vec3 calculateDiffuse(vec3 lightColor, vec3 objectColor, vec3 lightDir, vec3 normal) {
    float diffuse = clamp(dot(-lightDir, normal), 0.0, 1.0);
    vec3 diffuseColor = lightColor * diffuse * objectColor;
    
    return diffuseColor;
}

//计算镜面反射
vec3 calculateSpecular(vec3 lightColor, vec3 lightDir, vec3 normal, vec3 viewDir, float intensity) {
    //1 防止背面光效果
    float dotResult = dot(-lightDir, normal);
    float flag = step(0.0, dotResult);
    
    vec3 lightReflect = normalize(reflect(lightDir, normal));
    
    //2 计算specular
    float specular = max(dot(lightReflect, -viewDir), 0.0);
    
    //3 控制光斑大小
    specular = pow(specular, shiness);
    
    float specularMask = texture(opacityMask, uv).r;
    
    //4 计算最终颜色
    vec3 specularColor = lightColor * specular * flag * intensity * specularMask;
    
    return specularColor;
}

vec3 calculateSpotLight(SpotLight light, vec3 normal, vec3 viewDir) {
    //  计算光照的通用数据
    vec3 objectColor = texture(sampler, uv).xyz;
    vec3 lightDir = normalize(worldPosition - light.position);
    vec3 targetDir = normalize(light.targetDirection);
    
    //计算spotlight的照射范围
    float cGamma = dot(lightDir, targetDir);
    float intensity = clamp((cGamma - light.outerLine) / (light.innerLine - light.outerLine), 0.0, 1.0);
    
    //1 计算diffuse
    vec3 diffuseColor = calculateDiffuse(light.color, objectColor, lightDir, normal);
    
    //2 计算specular
    vec3 specularColor = calculateSpecular(light.color, lightDir, normal, viewDir, light.specularIntensity);
    
    return (diffuseColor + specularColor) * intensity;
}

vec3 calculateDirectionalLight(vec3 objectColor, DirectionalLight light, vec3 normal, vec3 viewDir) {
    //  计算光照的通用数据
    vec3 lightDir = normalize(light.direction);
    
    //1 计算diffuse
    vec3 diffuseColor = calculateDiffuse(light.color, objectColor, lightDir, normal);
    
    //2 计算specular
    vec3 specularColor = calculateSpecular(light.color, lightDir, normal, viewDir, light.specularIntensity);
    
    return diffuseColor + specularColor;
}

vec3 calculatePointLight(vec3 objectColor, PointLight light, vec3 normal, vec3 viewDir) {
    //  计算光照的通用数据
//    vec3 objectColor = texture(sampler, uv).xyz;
    vec3 lightDir = normalize(worldPosition - light.position);
    
    // 计算衰减
    float dist = length(worldPosition - light.position);
    float attenuation = 1.0 / (light.k2 * dist * dist + light.k1 * dist + light.kc);
    
    //1 计算diffuse
    vec3 diffuseColor = calculateDiffuse(light.color, objectColor, lightDir, normal);
    
    //2 计算specular
    vec3 specularColor = calculateSpecular(light.color, lightDir, normal, viewDir, light.specularIntensity);
    
    return (diffuseColor + specularColor) * attenuation;
}


// ========== Shadow Map ==========
float calculateShadow(vec4 fragPosLightSpace) {
    // 透视除法 + 映射到 [0,1] UV 空间
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    
    // 超出shadow map范围的不算阴影
    if (projCoords.x > 1.0 || projCoords.x < 0.0 ||
        projCoords.y > 1.0 || projCoords.y < 0.0 ||
        projCoords.z > 1.0) {
        return 0.0;
    }
    
    float currentDepth = projCoords.z;
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    float bias = 0.002;
    
    //计算是否在阴影中
    float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
    return shadow;
}

void main() {
    vec2 worldUV = worldXZ / uvScale;
    vec3 objectColor = texture(sampler, worldUV).xyz * brightness;
    vec3 result = vec3(0.0f, 0.0f, 0.0f);
    
    //1 获取物体的当前像素的颜色
    //  计算光照的通用数据
    vec3 normalN = normalize(normal);
    vec3 viewDir = normalize(worldPosition - cameraPosition);
    
    result += calculateDirectionalLight(objectColor, directionalLight, normalN, viewDir);
    
    // Shadow Map 阴影（同时压暗漫反射和环境光）
    float shadow = calculateShadow(vertexPositionLightSpace);
    
    // 应用阴影
    float shadowFactor = 1.0 - shadow * shadowIntensity;
    result = result * shadowFactor;
    
    // 环境光计算
    
    float alpha = texture(opacityMask, uv).r;
    
    vec3 ambientColor = objectColor * ambientColor;
    
    vec3 grassColor = result + ambientColor * shadowFactor;
    
    vec3 windDirN = normalize(windDirection);
    vec2 cloudUV = worldXZ / cloudUVScale;
    cloudUV = cloudUV + time * cloudSpeed * windDirN.xz;
    float cloudMask = texture(cloudMask, cloudUV).r;
    vec3 cloudColor = mix(cloudBlackColor, cloudWhiteColor, cloudMask);
    
    vec3 finalColor = mix(grassColor, cloudColor, cloudLerp);
    
    FragColor = vec4(finalColor, alpha * opacity);
    
}
 
 

