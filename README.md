# OpenGL Grass Renderer

一个从零实现的 OpenGL 渲染框架，支持实例化草地、实时风力系统、Shadow Map、PBR 材质、NPR 卡通渲染。

## 功能

- **自研渲染架构**：Material-Scene-Mesh-Renderer 分层体系，8+ 种材质自动 Shader 分发
- **大规模实例化渲染**：InstancedMesh + 实例矩阵排序，单帧 2500 株草地，1 次 Draw Call
- **实时风力系统**：Vertex Shader 内 sin 相位偏移模拟草地摆动，风向/强度/相位可调
- **云层阴影系统**：世界坐标 UV + 时间流动的云纹理采样，动态云层覆盖
- **Shadow Map**：方向光深度贴图，2048x2048 分辨率，4-tap PCF 软阴影
- **PBR 材质**：Cook-Torrance BRDF（GGX + Smith + Schlick），Metallic-Roughness 工作流
- **透明排序**：基于 Camera View Z 由远及近排序
- **ImGui 调试面板**：运行时调节 20+ 参数
- **NPR 卡通渲染**：Toon Shader + 背面膨胀描边

## 如何运行

1. 用 Xcode 打开 `TryGL.xcodeproj`
2. 按 ⌘R 运行

## 环境

- macOS (Apple Silicon / Intel)
- Xcode 15+
- C++17
- OpenGL 3.3 Core

## 参考

- LearnOpenGL
- GAMES101 / GAMES202（闫令琪）
