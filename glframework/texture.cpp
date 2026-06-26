//
//  texture.cpp
//  TryGL
//
//  Created by 邓和平 on 2025/9/7.
//
#include "texture.h"
#include "../application/stb_image.h"

std::map<std::string, Texture*> Texture::mTextureCache{};

Texture* Texture::createTexture(const std::string& path, unsigned int unit) {
    //1 检查是否缓存过本路径对应的纹理对象
    auto iter = mTextureCache.find(path);
    if(iter != mTextureCache.end()) {
        // 对于iterater(迭代器), first -> key, sconde -> value
        return iter -> second;
    }
    
    //2 如果本路径对应的texture没有生产过，则重新生成
    auto texture = new Texture(path, unit);
    mTextureCache[path] = texture;
    
    return texture;
}

Texture* Texture::createTextureFromMemory (
    const std::string& path,
    unsigned int unit,
    unsigned char* dataIn,
    uint32_t widthIn,
    uint32_t heightIn
){
    //1 检查是否缓存过本路径对应的纹理对象
    auto iter = mTextureCache.find(path);
    if(iter != mTextureCache.end()) {
        // 对于iterater(迭代器), first -> key, sconde -> value
        return iter -> second;
    }
    //2 如果本路径对应的texture没有生产过，则重新生成
    auto texture = new Texture(unit, dataIn, widthIn, heightIn);
    mTextureCache[path] = texture;
    
    return texture;
}

Texture* Texture::createColorAttachment(
    unsigned int width,
    unsigned int height,
    unsigned int unit
){
    return new Texture(width, height, unit);
}

Texture* Texture::createDepthStencilAttachment(
    unsigned int width,
    unsigned int height,
    unsigned int unit
) {
    Texture* dsTex = new Texture();
    
    unsigned int depthStencil;
    glGenTextures(1, &depthStencil);
    glBindTexture(GL_TEXTURE_2D, depthStencil);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, width, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depthStencil, 0);
    
    dsTex -> mTexture = depthStencil;
    dsTex -> mWidth = width;
    dsTex -> mHeight = height;
    dsTex -> mUnit = unit;
    
    return dsTex;
}


Texture::Texture() {
    
}

Texture::Texture(const std::string& path, unsigned int unit) {
    mUnit = unit;
    //1 stbImage 读取图片
    int channels;
    
    // --反转y轴
    stbi_set_flip_vertically_on_load(true);
    
    unsigned char* data = stbi_load(path.c_str(), &mWidth, &mHeight, &channels, STBI_rgb_alpha);
//    unsigned char* data = stbi_load("assets/texture/asuka.jpg", &mWidth, &mHeight, &channels, STBI_rgb_alpha);
    
    if (!data) {
        std::cerr << "纹理加载失败: " << stbi_failure_reason() << std::endl;
        // 处理错误
    }
    
    //2 生成纹理并且激活单元绑定
    glGenTextures(1, &mTexture);
    //--激活纹理单元--
    glActiveTexture(GL_TEXTURE0 + mUnit);
    //--绑定纹理对象--
    glBindTexture(GL_TEXTURE_2D, mTexture);
    
    //3 传输纹理数据,开辟显存
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mWidth, mHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    
//    glGenerateMipmap(GL_TEXTURE_2D);
    
    
    //***释放数据
    stbi_image_free(data);
    
    //4 设置纹理的过滤方式
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    
    //5 设置纹理的包裹方式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // u
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // v
}


Texture::Texture(
    unsigned int unit,
    unsigned char* dataIn,
    uint32_t widthIn,
    uint32_t heightIn
) {
    mUnit = unit;
    
    //1 stbImage 读取图片
    int channels;
    
    // --反转y轴
    stbi_set_flip_vertically_on_load(true);
    
    // 计算整张图片的大小
    // Assimp规定:如果内嵌纹理是png或者jpg压缩格式的话，height = 0，width就代表了图片的大小
    uint32_t dataInSize = 0;
    if (!heightIn) {
        dataInSize = widthIn;
    } else {
        // 如果内嵌图片不是压缩格式，height = 正常高度，width = 正常宽度
        // 偷懒：统一认为数据格式都是RGBA
        dataInSize = widthIn * heightIn * 4; // 字节为单位
    }
    
    
    unsigned char* data = stbi_load_from_memory(dataIn, dataInSize, &mWidth, &mHeight, &channels, STBI_rgb_alpha);
    
    if (!data) {
        std::cerr << "纹理加载失败: " << stbi_failure_reason() << std::endl;
        // 处理错误
    }
    
    //2 生成纹理并且激活单元绑定
    glGenTextures(1, &mTexture);
    //--激活纹理单元--
    glActiveTexture(GL_TEXTURE0 + mUnit);
    //--绑定纹理对象--
    glBindTexture(GL_TEXTURE_2D, mTexture);
    
    //3 传输纹理数据,开辟显存
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mWidth, mHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    
    glGenerateMipmap(GL_TEXTURE_2D);
    
    
    //***释放数据
    stbi_image_free(data);
    
    //4 设置纹理的过滤方式
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    
    //5 设置纹理的包裹方式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // u
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // v
}

Texture::Texture(unsigned int width, unsigned int height, unsigned int unit) {
    mWidth = width;
    mHeight = height;
    mUnit = unit;

    glGenTextures(1, &mTexture);
    glActiveTexture(GL_TEXTURE0 + mUnit);
    glBindTexture(GL_TEXTURE_2D, mTexture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mWidth, mHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}


// paths:右左上下后前（+x -x +y -y +z -z）
Texture::Texture(const std::vector<std::string>& paths, unsigned int unit) {
    mUnit = unit;
    mTextureTarget = GL_TEXTURE_CUBE_MAP;
    
    // CubeMap不用反转y轴
    stbi_set_flip_vertically_on_load(false);
    
    //1 创建CubeMap对象
    glGenTextures(1, &mTexture);
    glActiveTexture(GL_TEXTURE0 + mUnit);
    glBindTexture(GL_TEXTURE_CUBE_MAP, mTexture);
    
    //2 循环读取六张贴图，并且放置到 cubemap的六个GPU空间内
    int channels;
    int width = 0, height = 0;
    unsigned char* data = nullptr;
    for (int i = 0; i < paths.size(); i ++) {
        data = stbi_load(paths[i].c_str(), &width, &height, &channels, STBI_rgb_alpha);
        if (data != nullptr) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        } else {
            std::cout << "Error: CubeMap Texture failed to load at path - " << paths[i] << std::endl;
            stbi_image_free(data);
        }
    }
    
    //3 设置纹理参数
    glTexParameterf(mTextureTarget, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(mTextureTarget, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(mTextureTarget, GL_TEXTURE_WRAP_S, GL_REPEAT); // u
    glTexParameteri(mTextureTarget, GL_TEXTURE_WRAP_T, GL_REPEAT); // v
    
}

Texture::~Texture() {
    if (mTexture != 0) {
        glDeleteTextures(1, &mTexture);
    }
}

void Texture::bind() {
    // 先切换纹理单元，然后绑定texture对象
    glActiveTexture(GL_TEXTURE0 + mUnit);
    glBindTexture(mTextureTarget, mTexture);
}
