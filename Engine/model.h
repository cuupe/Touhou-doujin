#pragma once
#include "../prefix.h"
namespace Engine {
    using Microsoft::WRL::ComPtr;
    // 顶点结构（匹配 D3D11Renderer 的输入布局）
    struct Vertex {
        DirectX::XMFLOAT3 pos;      // 位置 (12 bytes)
        DirectX::XMFLOAT3 normal;   // 法线 (12 bytes)
        DirectX::XMFLOAT2 uv;       // UV 坐标 (8 bytes)
        DirectX::XMFLOAT3 tangent;  // 切线 (12 bytes, 可选，用于法线贴图)
        // 总 44 bytes（D3D11 要求 4 字节对齐，可用 padding 调整）
    };

    // 网格结构（每个 Assimp aiMesh 对应一个）
    struct Mesh {
        ComPtr<ID3D11Buffer> vertexBuffer;     // VBO
        ComPtr<ID3D11Buffer> indexBuffer;      // IBO
        ComPtr<ID3D11InputLayout> inputLayout; // 输入布局
        UINT indexCount = 0;                   // 索引数量
        std::string materialName;              // 材质名（用于纹理绑定）

        // 构造函数：从 aiMesh 创建 D3D11 资源
        Mesh(ID3D11Device* device, ComPtr<ID3DBlob>& vsBlob, const aiMesh* mesh);

        // 绘制方法（绑定并 DrawIndexed）
        void Draw(ID3D11DeviceContext* context) const;
    };

    // 材质结构（简化：颜色 + 纹理路径）
    struct Material {
        DirectX::XMFLOAT4 diffuseColor = { 1.0f, 1.0f, 1.0f, 1.0f };  // 漫反射色
        std::string diffuseTexturePath;  // 纹理路径
        // 可扩展：specular, normalMap 等
    };

    class Model {
    private:
        std::vector<Mesh> meshes;           // 所有网格
        std::vector<Material> materials;    // 所有材质
        std::string directory;              // 模型文件目录（用于相对纹理路径）

        // 私有方法：加载 Assimp 场景
        bool LoadAssimpScene(const std::string& path, ID3D11Device* device, ComPtr<ID3DBlob>& vsBlob);

        // 处理节点（递归遍历场景树）
        void ProcessNode(const aiNode* node, const aiScene* scene, ID3D11Device* device, ComPtr<ID3DBlob>& vsBlob);

        // 处理单个网格
        void ProcessMesh(const aiMesh* mesh, const aiScene* scene, ID3D11Device* device, ComPtr<ID3DBlob>& vsBlob);

        // 处理材质
        void ProcessMaterial(const aiMaterial* material, const std::string& dir);

    public:
        Model(ID3D11Device* device, const std::string& path, ComPtr<ID3DBlob>& vsBlob);
        ~Model();

        // 绘制所有网格
        void Draw(ID3D11DeviceContext* context) const;
        // 获取网格/材质数量
        size_t GetMeshCount() const { return meshes.size(); }
        size_t GetMaterialCount() const { return materials.size(); }

        // 获取材质（用于纹理加载）
        const Material& GetMaterial(size_t index) const { return materials[index]; }
        const Mesh& GetMesh(size_t index) const { return meshes[index]; }
    };
}