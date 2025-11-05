//#pragma once
//#include "../prefix.h"
//
//namespace Engine {
//    using Microsoft::WRL::ComPtr;
//    class Model;  // Assimp 模型类
//
//    // 常量缓冲结构（扩展：矩阵 + 光源 + 材质参数）
//    // 注意：D3D11 CB 需 16 字节对齐
//    struct ConstantBufferPerObject {
//        DirectX::XMFLOAT4X4 world;
//        DirectX::XMFLOAT4X4 worldInvTranspose;  // 用于法线变换
//        DirectX::XMFLOAT4 materialColor;  // 材质颜色 (RGBA)
//    };
//
//    // 常量缓冲结构（Per Frame：视图/投影 + 光源 + 时间 + 相机）
//    // 总大小：176 bytes (16 字节对齐)
//    struct ConstantBufferPerFrame {
//        DirectX::XMFLOAT4X4 view;           // 64 bytes
//        DirectX::XMFLOAT4X4 proj;           // 64 bytes
//        DirectX::XMFLOAT3 lightDir[2];      // 24 bytes (2 个方向光源)
//        float time;                         // 4 bytes (用于动画)
//        DirectX::XMFLOAT3 eyePos;           // 12 bytes (相机位置)
//        float padding[4];                   // 16 bytes (对齐到 176 bytes，确保下一个 CB 从 16 字节边界开始)
//    };
//
//    // 纹理采样器描述（简单线性过滤）
//    struct SamplerDesc {
//        D3D11_FILTER filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
//        D3D11_TEXTURE_ADDRESS_MODE addressU = D3D11_TEXTURE_ADDRESS_WRAP;
//        D3D11_TEXTURE_ADDRESS_MODE addressV = D3D11_TEXTURE_ADDRESS_WRAP;
//        D3D11_TEXTURE_ADDRESS_MODE addressW = D3D11_TEXTURE_ADDRESS_WRAP;
//        float mipLODBias = 0.0f;
//        UINT maxAnisotropy = 1;
//        D3D11_COMPARISON_FUNC comparisonFunc = D3D11_COMPARISON_NEVER;
//        DirectX::XMFLOAT4 borderColor = { 0.0f, 0.0f, 0.0f, 0.0f };
//        float minLOD = 0.0f;
//        float maxLOD = D3D11_FLOAT32_MAX;
//    };
//
//    class D3D11Renderer {
//    public:
//        const std::string doc{ "debug" };
//
//    private:
//        ComPtr<ID3D11Device> device;
//        ComPtr<ID3D11DeviceContext> context;
//
//        // 着色器
//        ComPtr<ID3D11VertexShader> vertexShader;
//        ComPtr<ID3D11PixelShader> pixelShader;
//        ComPtr<ID3DBlob> vsBlob;  // VS 字节码，用于输入布局
//
//        // 常量缓冲（Per Object 和 Per Frame）
//        ComPtr<ID3D11Buffer> cbPerObject;
//        ComPtr<ID3D11Buffer> cbPerFrame;
//
//        // 纹理资源
//        ComPtr<ID3D11SamplerState> samplerState;  // 采样器（共享）
//        std::vector<ComPtr<ID3D11ShaderResourceView>> textureSRVs;  // 纹理视图数组
//
//        // 渲染状态
//        ComPtr<ID3D11DepthStencilState> depthStencilState;
//        ComPtr<ID3D11RasterizerState> rasterizerState;
//        ComPtr<ID3D11BlendState> blendState;
//        ComPtr<ID3D11InputLayout> inputLayout;  // 全局输入布局（假设统一顶点格式）
//
//        // 矩阵缓存
//        DirectX::XMMATRIX viewMatrix;
//        DirectX::XMMATRIX projMatrix;
//
//        // 调试标志
//        bool debugMode = false;  // 启用 D3D11_DEBUG
//
//        // 私有方法
//        bool CompileShaders();
//        bool CreateConstantBuffers();
//        bool CreateSamplerState();
//        bool CreateRenderStates();
//        bool CreateInputLayout();
//        void UpdatePerObjectCB(const DirectX::XMMATRIX& world, const DirectX::XMFLOAT4& materialColor = { 1.0f, 1.0f, 1.0f, 1.0f });
//        void UpdatePerFrameCB(const DirectX::XMFLOAT3& lightDir1 = { 0.0f, 1.0f, -1.0f }, const DirectX::XMFLOAT3& lightDir2 = { -1.0f, -1.0f, 1.0f }, float time = 0.0f, const DirectX::XMFLOAT3& eyePos = { 0.0f, 0.0f, 3.0f });
//        bool LoadTexture(const std::string& filePath, ComPtr<ID3D11ShaderResourceView>& srv);
//
//    public:
//        D3D11Renderer(ID3D11Device* dev, ID3D11DeviceContext* ctx, bool enableDebug = false);
//        D3D11Renderer(const D3D11Renderer&) = delete;
//        D3D11Renderer(D3D11Renderer&&) = delete;
//        ~D3D11Renderer();
//
//        // 初始化所有资源
//        bool Initialize(float fov = 45.0f, float nearZ = 0.1f, float farZ = 100.0f, float aspect = 800.0f / 600.0f);
//
//        // 渲染模型（支持纹理）
//        void Render(const Model& model, const DirectX::XMMATRIX& worldMatrix, const DirectX::XMFLOAT4& materialColor = { 1.0f, 1.0f, 1.0f, 1.0f }, float deltaTime = 0.0f);
//
//        // 设置视图/投影矩阵
//        void SetViewProjection(const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& proj);
//
//        // 加载纹理到 SRV 数组（索引对应模型纹理）
//        bool LoadModelTextures(const Model& model, const std::vector<std::string>& texturePaths);
//
//        // 启用/禁用调试层
//        void SetDebugMode(bool enable);
//
//        ID3D11DeviceContext* GetContext() const { return context.Get(); }
//        const ComPtr<ID3D11SamplerState>& GetSampler() const { return samplerState; }
//    };
//}