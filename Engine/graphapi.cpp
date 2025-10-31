#include "graphapi.h"
#include "model.h"
namespace Engine {
    // 着色器编译
    ComPtr<ID3DBlob> CompileShader(const char* hlslCode, const char* entryPoint, const char* shaderModel, ID3D11Device* device) {
        ComPtr<ID3DBlob> blob;
        ComPtr<ID3DBlob> errorBlob;
        // 编译标志：调试信息 + 优化
        UINT compileFlags = D3DCOMPILE_ENABLE_STRICTNESS;
        if (device && device->GetFeatureLevel() < D3D_FEATURE_LEVEL_11_0) {
            compileFlags |= D3DCOMPILE_PACK_MATRIX_COLUMN_MAJOR;  // 旧设备兼容
        }
        HRESULT hr = D3DCompile(hlslCode, strlen(hlslCode), nullptr, nullptr, nullptr, entryPoint, shaderModel, compileFlags, 0, &blob, &errorBlob);
        if (FAILED(hr)) {
            if (errorBlob) {
                char* errMsg = (char*)errorBlob->GetBufferPointer();
                OutputDebugStringA(errMsg);  // VS 输出窗口
                std::cout << "HLSL 编译失败 (" << entryPoint << "): " << errMsg << std::endl;
            }
            else {
                std::cout << "HLSL 编译未知错误: 0x" << std::hex << hr << std::endl;
            }
            return nullptr;
        }
        std::cout << "HLSL 编译成功 (" << entryPoint << ")" << std::endl;
        return blob;
    }

    D3D11Renderer::D3D11Renderer(ID3D11Device* dev, ID3D11DeviceContext* ctx, bool enableDebug)
        : device(dev), context(ctx), debugMode(enableDebug) {
        if (!device || !context) {
            std::cout << "警告: 无效 Device 或 Context 传入 D3D11Renderer" << std::endl;
        }
        if (debugMode) {
            std::cout << "D3D11Renderer: 调试模式启用（需重建 Device 以支持 D3D11_DEBUG）" << std::endl;
        }
    }

    D3D11Renderer::~D3D11Renderer() {
        textureSRVs.clear();  // 释放纹理 SRV
        std::cout << "D3D11Renderer 析构：所有 ComPtr 已自动释放" << std::endl;
    }

    bool D3D11Renderer::Initialize(float fov, float nearZ, float farZ, float aspect) {
        if (!CompileShaders()) return false;
        if (!CreateInputLayout()) return false;
        if (!CreateConstantBuffers()) return false;
        if (!CreateSamplerState()) return false;
        if (!CreateRenderStates()) return false;

        // 设置默认投影矩阵
        projMatrix = DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(fov), aspect, nearZ, farZ);
        viewMatrix = DirectX::XMMatrixIdentity();  // 默认视图，后续设置

        std::cout << "D3D11Renderer 初始化完成 (FOV: " << fov << ", Aspect: " << aspect << ")" << std::endl;
        return true;
    }

    bool D3D11Renderer::CompileShaders() {
        //TODO:
        return true;
    }

    bool D3D11Renderer::CreateInputLayout() {
        // 输入元素描述（匹配 VS_INPUT）
        D3D11_INPUT_ELEMENT_DESC layout[] = {
            {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0}  // 可选
        };
        UINT numElements = _countof(layout);

        HRESULT hr = device->CreateInputLayout(layout, numElements, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &inputLayout);
        if (FAILED(hr)) {
            std::cout << "输入布局创建失败: 0x" << std::hex << hr << std::endl;
            return false;
        }
        return true;
    }

    bool D3D11Renderer::CreateConstantBuffers() {
        // Per Object CB
        D3D11_BUFFER_DESC desc = {};
        desc.ByteWidth = sizeof(ConstantBufferPerObject);
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        HRESULT hr = device->CreateBuffer(&desc, nullptr, &cbPerObject);
        if (FAILED(hr)) {
            std::cout << "Per Object CB 创建失败: 0x" << std::hex << hr << std::endl;
            return false;
        }

        // Per Frame CB
        desc.ByteWidth = sizeof(ConstantBufferPerFrame);
        hr = device->CreateBuffer(&desc, nullptr, &cbPerFrame);
        if (FAILED(hr)) {
            std::cout << "Per Frame CB 创建失败: 0x" << std::hex << hr << std::endl;
            return false;
        }

        return true;
    }

    bool D3D11Renderer::CreateSamplerState() {
        D3D11_SAMPLER_DESC sampDesc = {};
        sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
        sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
        sampDesc.MipLODBias = 0.0f;
        sampDesc.MaxAnisotropy = 1;
        sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
        sampDesc.BorderColor[0] = sampDesc.BorderColor[1] = sampDesc.BorderColor[2] = sampDesc.BorderColor[3] = 0.0f;
        sampDesc.MinLOD = 0;
        sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

        HRESULT hr = device->CreateSamplerState(&sampDesc, &samplerState);
        if (FAILED(hr)) {
            std::cout << "采样器创建失败: 0x" << std::hex << hr << std::endl;
            return false;
        }
        return true;
    }

    bool D3D11Renderer::CreateRenderStates() {
        // 深度模板：启用 < 比较，写深度
        D3D11_DEPTH_STENCIL_DESC dsDesc = {};
        dsDesc.DepthEnable = TRUE;
        dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
        dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
        dsDesc.StencilEnable = FALSE;
        HRESULT hr = device->CreateDepthStencilState(&dsDesc, &depthStencilState);
        if (FAILED(hr)) return false;

        // 光栅化：固体填充，背面剔除，顺时针正面
        D3D11_RASTERIZER_DESC rsDesc = {};
        rsDesc.FillMode = D3D11_FILL_SOLID;
        rsDesc.CullMode = D3D11_CULL_BACK;
        rsDesc.FrontCounterClockwise = FALSE;  // D3D11 左手系，顺时针正面
        rsDesc.DepthBias = 0;
        rsDesc.DepthBiasClamp = 0.0f;
        rsDesc.SlopeScaledDepthBias = 0.0f;
        rsDesc.DepthClipEnable = TRUE;
        rsDesc.ScissorEnable = FALSE;
        rsDesc.MultisampleEnable = FALSE;
        rsDesc.AntialiasedLineEnable = FALSE;
        hr = device->CreateRasterizerState(&rsDesc, &rasterizerState);
        if (FAILED(hr)) return false;

        // 混合：禁用（不透明）
        D3D11_BLEND_DESC blendDesc = {};
        blendDesc.AlphaToCoverageEnable = FALSE;
        blendDesc.IndependentBlendEnable = FALSE;
        blendDesc.RenderTarget[0].BlendEnable = FALSE;
        blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
        blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
        blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
        blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
        blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
        blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
        blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
        hr = device->CreateBlendState(&blendDesc, &blendState);
        if (FAILED(hr)) return false;

        return true;
    }

    void D3D11Renderer::SetViewProjection(const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& proj) {
        viewMatrix = view;
        projMatrix = proj;
        std::cout << "视图/投影矩阵更新" << std::endl;
    }

    bool D3D11Renderer::LoadTexture(const std::string& filePath, ComPtr<ID3D11ShaderResourceView>& srv) {
        try {
            DirectX::TexMetadata metadata;
            DirectX::ScratchImage image;

            // 宽字符串转换 (filePath 是 std::string -> std::wstring)
            std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
            std::wstring widePath = converter.from_bytes(filePath);

            // 加载纹理数据 (WIC for PNG/JPG/BMP)
            // 对于 DDS: 用 DirectX::LoadFromDDSFile(widePath.c_str(), DirectX::DDS_FLAGS_NONE, &metadata, image);
            HRESULT hr = DirectX::LoadFromWICFile(
                widePath.c_str(),           // 宽路径
                DirectX::WIC_FLAGS_NONE,    // 标志: 默认
                &metadata,                  // 元数据输出 (指针)
                image                       // ScratchImage 输出 (引用)
            );
            if (FAILED(hr)) {
                std::cout << "LoadFromWICFile 失败 (" << filePath << "): 0x" << std::hex << hr << std::endl;
                return false;
            }

            // 创建 D3D11 纹理资源 (从 ScratchImage)
            ComPtr<ID3D11Resource> texture;
            hr = DirectX::CreateTexture(device.Get(), image.GetImages(), image.GetImageCount(), metadata, texture.GetAddressOf());
            if (FAILED(hr)) {
                std::cout << "CreateTexture 失败: 0x" << std::hex << hr << std::endl;
                return false;
            }

            // 手动创建 SRV (D3D11 API: 从现有 texture + metadata 推导描述符)
            D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
            srvDesc.Format = metadata.format;  // 从 metadata 复制格式
            srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;  // 假设 2D 纹理 (调整为 CUBE/3D 等)

            // 根据 metadata 配置维度参数
            srvDesc.Texture2D.MipLevels = (metadata.mipLevels > 0) ? metadata.mipLevels : 1;  // Mip 级别
            srvDesc.Texture2D.MostDetailedMip = 0;  // 最详细 Mip (0 = 基础)

            ComPtr<ID3D11ShaderResourceView> tempSRV;
            hr = device->CreateShaderResourceView(texture.Get(), &srvDesc, tempSRV.GetAddressOf());  // D3D11 API 调用
            if (FAILED(hr)) {
                std::cout << "CreateShaderResourceView 失败: 0x" << std::hex << hr << std::endl;
                return false;
            }

            srv = tempSRV;  // 转移到输出
            std::cout << "纹理 + SRV 创建成功 (" << filePath << ", " << metadata.width << "x" << metadata.height << ")" << std::endl;
            return true;

        }
        catch (const std::exception& e) {
            std::cout << "纹理异常: " << e.what() << std::endl;
            return false;
        }
    }

    bool D3D11Renderer::LoadModelTextures(const Model& model, const std::vector<std::string>& texturePaths) {
        
        if (texturePaths.size() != model.GetMeshCount()) {
            std::cout << "纹理路径数量不匹配网格数量" << std::endl;
            return false;
        }

        textureSRVs.clear();
        textureSRVs.resize(texturePaths.size());

        for (size_t i = 0; i < texturePaths.size(); ++i) {
            if (!LoadTexture(texturePaths[i], textureSRVs[i])) {
                std::cout << "模型纹理 " << i << " 加载失败" << std::endl;
                return false;
            }
        }
        std::cout << "模型纹理加载完成 (" << texturePaths.size() << " 张)" << std::endl;
        return true;
    }

    void D3D11Renderer::UpdatePerObjectCB(const DirectX::XMMATRIX& world, const DirectX::XMFLOAT4& materialColor) {
        ConstantBufferPerObject cb;
        DirectX::XMStoreFloat4x4(&cb.world, XMMatrixTranspose(world));  // D3D 行主序
        // 逆转置用于法线
        DirectX::XMMATRIX worldInv = XMMatrixInverse(nullptr, world);
        DirectX::XMStoreFloat4x4(&cb.worldInvTranspose, XMMatrixTranspose(worldInv));
        cb.materialColor = materialColor;

        context->UpdateSubresource(cbPerObject.Get(), 0, nullptr, &cb, 0, 0);
    }

    void D3D11Renderer::UpdatePerFrameCB(const DirectX::XMFLOAT3& lightDir1,
        const DirectX::XMFLOAT3& lightDir2,
        float time,
        const DirectX::XMFLOAT3& eyePos) {
        ConstantBufferPerFrame cb = {};  // 初始化为零

        // 存储转置矩阵（HLSL 列主序）
        DirectX::XMStoreFloat4x4(&cb.view, DirectX::XMMatrixTranspose(viewMatrix));
        DirectX::XMStoreFloat4x4(&cb.proj, DirectX::XMMatrixTranspose(projMatrix));

        // 填充数据
        cb.lightDir[0] = lightDir1;
        cb.lightDir[1] = lightDir2;
        cb.time = time;
        cb.eyePos = eyePos;

        // 填充 padding（数组，确保对齐）
        cb.padding[0] = cb.padding[1] = cb.padding[2] = cb.padding[3] = 0.0f;

        // 直接调用（无返回值）
        context->UpdateSubresource(cbPerFrame.Get(), 0, nullptr, &cb, 0, 0);

        // 可选：检查设备状态（非阻塞错误检测）
        HRESULT deviceReason = device->GetDeviceRemovedReason();
        if (deviceReason != S_OK) {
            std::cout << "设备移除警告 (UpdateSubresource 前): 0x" << std::hex << deviceReason << std::endl;
            // 处理：重置 device/context
            exit(0);
        }

        // 调试输出（可选，生产中移除）
        std::cout << "PerFrameCB 更新完成 (时间: " << time << ", 光源1: " << lightDir1.x << "," << lightDir1.y << "," << lightDir1.z << ")" << std::endl;
    }

    void D3D11Renderer::Render(const Model& model, const DirectX::XMMATRIX& worldMatrix, const DirectX::XMFLOAT4& materialColor, float deltaTime) {
        if (cbPerObject == nullptr || cbPerFrame == nullptr) {
            std::cout << "渲染失败：CB 未初始化" << std::endl;
            return;
        }

        // 更新 CB（Per Frame 每帧，Per Object 每对象）
        static float totalTime = 0.0f;
        totalTime += deltaTime;
        UpdatePerFrameCB({ 0.0f, 1.0f, -1.0f }, { -1.0f, -1.0f, 1.0f }, totalTime, { 0.0f, 0.0f, 3.0f });  // 示例光源/相机
        UpdatePerObjectCB(worldMatrix, materialColor);

        // 清屏（假设 SDL 提供 RTV/DSV；nullptr 表示当前）
        float clearColor[4] = { 0.2f, 0.3f, 0.3f, 1.0f };
        context->ClearRenderTargetView(nullptr, clearColor);
        context->ClearDepthStencilView(nullptr, D3D11_CLEAR_DEPTH, 1.0f, 0);

        // 设置管道状态
        context->OMSetDepthStencilState(depthStencilState.Get(), 1);  // 深度引用值 1
        context->RSSetState(rasterizerState.Get());
        context->OMSetBlendState(blendState.Get(), nullptr, 0xFFFFFFFF);  // 全通道写

        // 绑定着色器
        context->VSSetShader(vertexShader.Get(), nullptr, 0);
        context->PSSetShader(pixelShader.Get(), nullptr, 0);

        // 绑定 CB
        ID3D11Buffer* cbVS[2] = { cbPerObject.Get(), cbPerFrame.Get() };
        ID3D11Buffer* cbPS[2] = { cbPerObject.Get(), cbPerFrame.Get() };
        context->VSSetConstantBuffers(0, 2, cbVS);
        context->PSSetConstantBuffers(0, 2, cbPS);

        // 绑定输入布局
        context->IASetInputLayout(inputLayout.Get());

        // 绑定采样器（PS slot 0）
        context->PSSetSamplers(0, 1, samplerState.GetAddressOf());


        for (size_t i = 0; i < model.GetMeshCount(); ++i) {
            const auto& mesh = model.GetMesh(i);
            UINT stride = sizeof(Vertex);
            UINT offset = 0;
            ID3D11Buffer* vb = mesh.vertexBuffer.Get();
            context->IASetVertexBuffers(0, 1, &vb, &stride, &offset);
            context->IASetIndexBuffer(mesh.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
            context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

            // 绑定纹理 SRV (PS slot 0)
            if (i < textureSRVs.size() && textureSRVs[i]) {
                ID3D11ShaderResourceView* srv = textureSRVs[i].Get();
                context->PSSetShaderResources(0, 1, &srv);
            }
            else {
                ID3D11ShaderResourceView* nullSRV = nullptr;
                context->PSSetShaderResources(0, 1, &nullSRV);
            }

            context->DrawIndexed(mesh.indexCount, 0, 0);
        }

        std::cout << "渲染一帧完成" << std::endl;
    }

    void D3D11Renderer::SetDebugMode(bool enable) {
        debugMode = enable;
        if (enable) {
            std::cout << "调试模式已设置（重建 Device 以启用完整调试）" << std::endl;
        }
    }
}