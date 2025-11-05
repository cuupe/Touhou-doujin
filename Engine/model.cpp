//#pragma once
//#include "model.h"
//namespace Engine {
//    // Mesh 构造函数实现
//    Mesh::Mesh(ID3D11Device* device, ComPtr<ID3DBlob>& vsBlob, const aiMesh* mesh) {
//        // 步骤 1: 提取顶点数据
//        std::vector<Vertex> vertices;
//        vertices.reserve(mesh->mNumVertices);
//
//        for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
//            Vertex vertex = {};
//            // 位置
//            vertex.pos = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };
//            // 法线
//            if (mesh->HasNormals()) {
//                vertex.normal = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };
//            }
//            // UV
//            if (mesh->mTextureCoords[0]) {
//                vertex.uv = { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };
//            }
//            else {
//                vertex.uv = { 0.0f, 0.0f };
//            }
//            // 切线（如果有，Assimp postprocess 生成）
//            if (mesh->mTangents) {
//                vertex.tangent = { mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z };
//            }
//            else {
//                vertex.tangent = { 0.0f, 0.0f, 0.0f };
//            }
//            vertices.push_back(vertex);
//        }
//
//        // 步骤 2: 提取索引数据
//        std::vector<UINT> indices;
//        indices.reserve(mesh->mNumFaces * 3);  // 假设三角面
//        for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
//            const aiFace& face = mesh->mFaces[i];
//            for (unsigned int j = 0; j < face.mNumIndices; ++j) {
//                indices.push_back(face.mIndices[j]);
//            }
//        }
//        indexCount = static_cast<UINT>(indices.size());
//
//        // 步骤 3: 创建 VBO
//        D3D11_BUFFER_DESC vbDesc = {};
//        vbDesc.ByteWidth = sizeof(Vertex) * vertices.size();
//        vbDesc.Usage = D3D11_USAGE_DEFAULT;
//        vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
//        D3D11_SUBRESOURCE_DATA vbData = { vertices.data(), 0, 0 };
//        HRESULT hr = device->CreateBuffer(&vbDesc, &vbData, &vertexBuffer);
//        if (FAILED(hr)) {
//            std::cout << "VBO 创建失败: 0x" << std::hex << hr << std::endl;
//            return;
//        }
//
//        // 步骤 4: 创建 IBO
//        D3D11_BUFFER_DESC ibDesc = {};
//        ibDesc.ByteWidth = sizeof(UINT) * indices.size();
//        ibDesc.Usage = D3D11_USAGE_DEFAULT;
//        ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
//        D3D11_SUBRESOURCE_DATA ibData = { indices.data(), 0, 0 };
//        hr = device->CreateBuffer(&ibDesc, &ibData, &indexBuffer);
//        if (FAILED(hr)) {
//            std::cout << "IBO 创建失败: 0x" << std::hex << hr << std::endl;
//            return;
//        }
//
//        // 步骤 5: 创建输入布局（基于 VS 字节码）
//        D3D11_INPUT_ELEMENT_DESC layout[] = {
//            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
//            { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
//            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
//            { "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 }
//        };
//        UINT numElements = _countof(layout);
//        hr = device->CreateInputLayout(layout, numElements, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &inputLayout);
//        if (FAILED(hr)) {
//            std::cout << "输入布局创建失败: 0x" << std::hex << hr << std::endl;
//            return;
//        }
//
//        // 材质名（从网格索引获取）
//        materialName = mesh->mMaterialIndex >= 0 ? std::to_string(mesh->mMaterialIndex) : "default";
//
//        std::cout << "网格创建成功: " << mesh->mName.C_Str() << " (顶点: " << vertices.size() << ", 索引: " << indices.size() << ")" << std::endl;
//    }
//
//    // Mesh 绘制实现
//    void Mesh::Draw(ID3D11DeviceContext* context) const {
//        if (!vertexBuffer || !indexBuffer) return;
//
//        UINT stride = sizeof(Vertex);
//        UINT offset = 0;
//        ID3D11Buffer* vb = vertexBuffer.Get();
//        context->IASetVertexBuffers(0, 1, &vb, &stride, &offset);
//        context->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
//        context->IASetInputLayout(inputLayout.Get());
//        context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//
//        context->DrawIndexed(indexCount, 0, 0);
//        std::cout << "绘制网格: " << indexCount << " 索引" << std::endl;  // 调试
//    }
//
//    // Model 构造函数
//    Model::Model(ID3D11Device* device, const std::string& path, ComPtr<ID3DBlob>& vsBlob) {
//        // 提取目录
//        size_t lastSlash = path.find_last_of("/\\");
//        directory = (lastSlash != std::string::npos) ? path.substr(0, lastSlash + 1) : "./";
//
//        // 加载 Assimp 场景
//        if (!LoadAssimpScene(path, device, vsBlob)) {
//            std::cout << "模型加载失败: " << path << std::endl;
//        }
//        else {
//            std::cout << "模型加载成功: " << path << " (网格: " << meshes.size() << ", 材质: " << materials.size() << ")" << std::endl;
//        }
//    }
//
//    Model::~Model() {
//        std::cout << "Model 析构: 资源已自动释放" << std::endl;
//    }
//
//    bool Model::LoadAssimpScene(const std::string& path, ID3D11Device* device, ComPtr<ID3DBlob>& vsBlob) {
//        Assimp::Importer importer;
//        const aiScene* scene = importer.ReadFile(path,
//            aiProcess_Triangulate |          // 三角化所有面
//            aiProcess_FlipUVs |              // 翻转 V 坐标 (D3D11 左手系)
//            aiProcess_CalcTangentSpace |     // 计算切线/副切线 (用于法线贴图)
//            aiProcess_JoinIdenticalVertices | // 合并相同顶点
//            aiProcess_GenSmoothNormals |     // 平滑法线
//            aiProcess_OptimizeMeshes |       // 优化网格
//            aiProcess_SortByPType);          // 按图元类型排序
//
//        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
//            std::cout << "Assimp 读取失败: " << importer.GetErrorString() << std::endl;
//            return false;
//        }
//
//        // 处理根节点（递归）
//        ProcessNode(scene->mRootNode, scene, device, vsBlob);
//
//        // 处理材质
//        for (unsigned int i = 0; i < scene->mNumMaterials; ++i) {
//            ProcessMaterial(scene->mMaterials[i], directory);
//        }
//
//        return true;
//    }
//
//    void Model::ProcessNode(const aiNode* node, const aiScene* scene, ID3D11Device* device, ComPtr<ID3DBlob>& vsBlob) {
//        // 处理当前节点网格
//        for (unsigned int i = 0; i < node->mNumMeshes; ++i) {
//            unsigned int meshIndex = node->mMeshes[i];
//            ProcessMesh(scene->mMeshes[meshIndex], scene, device, vsBlob);
//        }
//
//        // 递归子节点
//        for (unsigned int i = 0; i < node->mNumChildren; ++i) {
//            ProcessNode(node->mChildren[i], scene, device, vsBlob);
//        }
//    }
//
//    void Model::ProcessMesh(const aiMesh* mesh, const aiScene* scene, ID3D11Device* device, ComPtr<ID3DBlob>& vsBlob) {
//        meshes.emplace_back(device, vsBlob, mesh);
//    }
//
//    void Model::ProcessMaterial(const aiMaterial* material, const std::string& dir) {
//        Material mat;
//        aiString path;
//
//        // 漫反射颜色（默认白）
//        aiColor3D color;
//        if (AI_SUCCESS == material->Get(AI_MATKEY_COLOR_DIFFUSE, color)) {
//            mat.diffuseColor = { color.r, color.g, color.b, 1.0f };
//        }
//
//        // 漫反射纹理路径
//        if (AI_SUCCESS == material->GetTexture(aiTextureType_DIFFUSE, 0, &path)) {
//            mat.diffuseTexturePath = dir + path.C_Str();
//        }
//
//        materials.push_back(mat);
//        std::cout << "材质处理: " << path.C_Str() << " (颜色: " << mat.diffuseColor.x << "," << mat.diffuseColor.y << "," << mat.diffuseColor.z << ")" << std::endl;
//    }
//
//    // Model 绘制实现
//    void Model::Draw(ID3D11DeviceContext* context) const {
//        for (const auto& mesh : meshes) {
//            mesh.Draw(context);
//        }
//    }
//}