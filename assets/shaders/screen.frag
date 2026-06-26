#version 330 core
out vec4 FragColor;

in vec2 uv;

uniform sampler2D screenTexSampler;

// 颜色反相
vec3 colorInvert(vec3 color) {
    vec3 invertColor = vec3(1.0) - color;
    return invertColor;
}

// 平均灰度
vec3 gray(vec3 color) {
    float avg = (color.r + color.g + color.b) / 3.0;
    return vec3(avg);
}

// 由于人眼对绿色更加敏感，对蓝色不敏感，为了物理精确
// 可以为每个颜色通道，增加权重值，让蓝色小一些，绿色多一些
vec3 grayCorrect(vec3 color) {
    float avg = color.r * 0.2126 + color.g * 0.7152 + color.b * 0.0722;
    return vec3(avg);
}

void main()
{
//    vec3 color = colorInvert(texture(screenTexSampler, uv).rgb);
//    vec3 color = gray(texture(screenTexSampler, uv).rgb);
//    vec3 color = grayCorrect(texture(screenTexSampler, uv).rgb);
//    FragColor = vec4(color, 1.0);
    FragColor = texture(screenTexSampler, uv);
}
