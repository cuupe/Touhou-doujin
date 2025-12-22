#include "resources.h"
#include "D3D/model.h"
#include "render.h"
#include "maths.h"
#ifdef max
#undef max
#endif
namespace Engine::Resource {
    using namespace Maths;
    ResourceManager::ResourceManager(ID3D11Device* device)
    {
        unsigned int white = 0xFFFFFFFF;
        D3D11_SUBRESOURCE_DATA init = { &white, 4, 0 };
        CD3D11_TEXTURE2D_DESC desc(DXGI_FORMAT_R8G8B8A8_UNORM, 1, 1);
        ComPtr<ID3D11Texture2D> tex;
        device->CreateTexture2D(&desc, &init, tex.GetAddressOf());
        device->CreateShaderResourceView(tex.Get(), nullptr, default_srv.GetAddressOf());
    }

    const glyph_data* ResourceManager::CacheGlyph(ID3D11DeviceContext* ctx, 
        const std::string& font_name, char32_t code_point) {
        auto font_it = fonts.find(font_name);
        if (font_it == fonts.end()) return nullptr;

        auto& font = font_it->second;
        if (font->glyphs.find(code_point) != font->glyphs.end()) {
            return &font->glyphs[code_point];
        }

        if (FT_Load_Char(font->face, code_point, FT_LOAD_RENDER)) {
            return nullptr;
        }

        FT_GlyphSlot slot = font->face->glyph;
        int w = slot->bitmap.width;
        int h = slot->bitmap.rows;

        int x, y;
        if (!font->try_pack(w, h, x, y)) {
            spdlog::warn("字体图集已满");
            return nullptr;
        }

        if (w > 0 && h > 0) {
            std::vector<uint32_t> buffer(w * h);
            for (int i = 0; i < w * h; ++i) {
                unsigned char alpha = slot->bitmap.buffer[i];
                buffer[i] = (static_cast<uint32_t>(alpha) << 24) | 0x00FFFFFF;
            }

            D3D11_BOX dest_box = { (UINT)x, (UINT)y, 0, (UINT)(x + w), (UINT)(y + h), 1 };
            ctx->UpdateSubresource(font->atlas_texture.Get(), 0, &dest_box, buffer.data(), w * 4, 0);
        }

        glyph_data data;
        data.u0 = (float)x / font->atlas_width;
        data.v0 = (float)y / font->atlas_height;
        data.u1 = (float)(x + w) / font->atlas_width;
        data.v1 = (float)(y + h) / font->atlas_height;
        data.width = (float)w;
        data.height = (float)h;
        data.x_offset = (float)slot->bitmap_left;
        data.y_offset = (float)slot->bitmap_top;
        data.advance = (float)(slot->advance.x >> 6);

        // 4. 存入缓存
        font->glyphs[code_point] = data;
        return &font->glyphs[code_point];
    }

    void ResourceManager::LoadTexture(SDL_Renderer* renderer, ID3D11Device* device, const std::string& path)
    {
        namespace fs = std::filesystem;
        std::filesystem::path fs_path = fs::u8path(path);
        std::string filename = fs_path.stem().string();
        if (!renderer) {
            spdlog::error("renderer为空，加载失败");
            return;
        }
        if (texs.find(filename) != texs.end()) {
            return;
        }

        SDL_Texture* texu = IMG_LoadTexture(renderer, path.c_str());
        if (!texu) {
            spdlog::error("加载纹理失败：{} ，SDL_image错误：{}", path.c_str(), SDL_GetError());
            throw std::runtime_error("发生错误");
            return;
        }

        int w, h;
        SDL_PropertiesID props = SDL_GetTextureProperties(texu);
        if (!props) {
            spdlog::error("获取纹理属性失败: {}，SDL错误：{}", path.c_str(), SDL_GetError());
            throw std::runtime_error("发生错误");
            return;
        }

        void* native_texture_ptr = SDL_GetPointerProperty(props, SDL_PROP_TEXTURE_D3D11_TEXTURE_POINTER, nullptr);
        if (!native_texture_ptr) {
            spdlog::error("获取纹理指针属性失败: {}，SDL错误：{}", path.c_str(), SDL_GetError());
            return;
        }
        ID3D11Texture2D* d3d11_texture = static_cast<ID3D11Texture2D*>(native_texture_ptr);

        D3D11_TEXTURE2D_DESC tex_desc;
        d3d11_texture->GetDesc(&tex_desc);

        D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc = {};
        srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srv_desc.Texture2D.MipLevels = 1;
        srv_desc.Texture2D.MostDetailedMip = 0;
        if (tex_desc.Format != DXGI_FORMAT_UNKNOWN) {
            srv_desc.Format = tex_desc.Format;
        }
        else {
            srv_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        }
        ComPtr<ID3D11ShaderResourceView> srv;
        HRESULT hr = device->CreateShaderResourceView(d3d11_texture, &srv_desc, srv.GetAddressOf());

        if (FAILED(hr)) {
            spdlog::error("CreateShaderResourceView 失败，HRESULT = 0x{:08X}", (uint32_t)hr);
            spdlog::error("纹理格式 = {}", (uint32_t)tex_desc.Format);
            return;
        }

        w = texu->w;
        h = texu->h;
        if (w <= 0 || h <= 0) {
            spdlog::error("提取纹理尺寸失败:w= {}， h= {}", w, h);
            throw std::runtime_error("发生错误");
            return;
        }

        TexturePtr te(texu);
        std::unique_ptr<TextureResource> tex_res = std::make_unique<TextureResource>();
        tex_res->height = h;
        tex_res->width = w;
        tex_res->texture = std::move(te);
        tex_res->d3d_srv = srv;

        texs.insert({ filename, std::move(tex_res) });
    }

    void ResourceManager::LoadAudio(MIX_Mixer* mixer, const std::string& path, bool predecode)
    {
        namespace fs = std::filesystem;
        std::filesystem::path fs_path(path);
        std::string filename = fs_path.stem().string();
        MIX_Audio* audio = MIX_LoadAudio(mixer, path.c_str(), predecode);
        if (!audio) {
            spdlog::error("加载音频失败: {}", SDL_GetError());
            throw std::runtime_error("发生错误");
        }
        std::unique_ptr<AudioResource> au = std::make_unique<AudioResource>();
        au->audio = MixAudioPtr(audio);

        audios.insert({ filename, std::move(au) });
    }


    void ResourceManager::LoadFont(ID3D11Device* device, FT_Library ft, const std::string& path, int size)
    {
        namespace fs = std::filesystem;

        std::string filename = fs::path(path).stem().string();

        if (fonts.find(filename) != fonts.end()) {
            return;
        }

        auto font_res = std::make_unique<FontResource>();

        if (FT_New_Face(ft, path.c_str(), 0, &font_res->face)) {
            spdlog::error("FreeType: 无法加载字体文件 {}", path);
            return;
        }
        FT_Set_Pixel_Sizes(font_res->face, 0, size);
        font_res->loaded_size = size;

        D3D11_TEXTURE2D_DESC tex_desc = {};
        tex_desc.Width = font_res->atlas_width;
        tex_desc.Height = font_res->atlas_height;
        tex_desc.MipLevels = 1;
        tex_desc.ArraySize = 1;
        tex_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // 修改为 RGBA
        tex_desc.SampleDesc.Count = 1;
        tex_desc.Usage = D3D11_USAGE_DEFAULT;
        tex_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        tex_desc.CPUAccessFlags = 0;

        if (FAILED(device->CreateTexture2D(&tex_desc, nullptr, font_res->atlas_texture.GetAddressOf()))) {
            spdlog::error("无法创建字体图集纹理");
            FT_Done_Face(font_res->face);
            return;
        }

        // 3. 创建 SRV
        D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc = {};
        srv_desc.Format = tex_desc.Format;
        srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srv_desc.Texture2D.MipLevels = 1;

        if (FAILED(device->CreateShaderResourceView(font_res->atlas_texture.Get(), &srv_desc, font_res->atlas_srv.GetAddressOf()))) {
            spdlog::error("无法创建字体SRV");
            return;
        }

        fonts[filename] = std::move(font_res);
        spdlog::info("成功加载字体: {}, 大小: {}", filename, size);
    }
    

    Render::D3D::ModelPtr ResourceManager::LoadModel(SDL_Renderer* render, ID3D11Device* device, const std::string& path)
    {
        std::string key = std::filesystem::path(path).stem().string();
        if (models.find(key) != models.end()) {
            return models[key];
        }

        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path,
            aiProcess_Triangulate |
            aiProcess_GenSmoothNormals |
            aiProcess_CalcTangentSpace |
            aiProcess_RemoveRedundantMaterials |
            aiProcess_ConvertToLeftHanded
        );

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            spdlog::error("Assimp 加载失败 {}: {}", path, importer.GetErrorString());
            return nullptr;
        }

        auto model = std::make_shared<Render::D3D::ModelResource>();
        model->name = key;

        std::string dir = std::filesystem::path(path).parent_path().string();
        if (!dir.empty() && dir.back() != '/' && dir.back() != '\\') dir += '/';

        std::function<void(aiNode*, const aiMatrix4x4&)> ProcessNode;
        ProcessNode =
            [&](aiNode* node, const aiMatrix4x4& parentTransform) {
            aiMatrix4x4 transform = parentTransform * node->mTransformation;
            if (scene->HasMeshes()) {
                for (unsigned int i = 0; i < node->mNumMeshes; ++i)
                {
                    aiMesh* aimesh = scene->mMeshes[node->mMeshes[i]];
                    Render::D3D::Mesh mesh;

                    DirectX::XMMATRIX node_transform = ai_mat_to_dx_mat(transform);

                    std::vector<Render::D3D::Vertex1> vertices;
                    vertices.reserve(aimesh->mNumVertices);
                    for (unsigned int v = 0; v < aimesh->mNumVertices; ++v)
                    {
                        Render::D3D::Vertex1 vert;
                        DirectX::XMVECTOR localPos = DirectX::XMVectorSet(
                            aimesh->mVertices[v].x,
                            aimesh->mVertices[v].y,
                            aimesh->mVertices[v].z,
                            1.0f
                        );

                        DirectX::XMVECTOR worldPos = DirectX::XMVector3TransformCoord(localPos, node_transform);

                        DirectX::XMStoreFloat3(&vert.pos, worldPos);
                        vert.tex = aimesh->HasTextureCoords(0) ?
                            DirectX::XMFLOAT2(aimesh->mTextureCoords[0][v].x, aimesh->mTextureCoords[0][v].y) :
                            DirectX::XMFLOAT2(0, 0);
                        vert.color = aimesh->HasVertexColors(0) ?
                            DirectX::XMFLOAT4(aimesh->mColors[0][v].r, aimesh->mColors[0][v].g,
                                aimesh->mColors[0][v].b, aimesh->mColors[0][v].a) :
                            DirectX::XMFLOAT4(1, 1, 1, 1);
                        if (aimesh->HasNormals()) {
                            DirectX::XMVECTOR local_norm = DirectX::XMVectorSet(
                                aimesh->mNormals[v].x, aimesh->mNormals[v].y, aimesh->mNormals[v].z, 0.0f);
                            DirectX::XMVECTOR world_norm = DirectX::XMVector3TransformNormal(local_norm, node_transform);
                            world_norm = DirectX::XMVector3Normalize(world_norm);
                            DirectX::XMStoreFloat3(&vert.norm, world_norm);
                        }
                        else {
                            vert.norm = DirectX::XMFLOAT3(0, 0, 1);
                        }

                        vertices.push_back(std::move(vert));
                    }

                    std::vector<UINT> indices;
                    for (unsigned int f = 0; f < aimesh->mNumFaces; ++f) {
                        for (unsigned int k = 0; k < aimesh->mFaces[f].mNumIndices; ++k) {
                            indices.emplace_back(aimesh->mFaces[f].mIndices[k]);
                        }
                    }

                    D3D11_BUFFER_DESC bd = { sizeof(Render::D3D::Vertex1) * (UINT)vertices.size(),
                        D3D11_USAGE_DEFAULT, D3D11_BIND_VERTEX_BUFFER };
                    D3D11_SUBRESOURCE_DATA initVB = { vertices.data() };
                    device->CreateBuffer(&bd, &initVB, mesh.vertex_buffer.GetAddressOf());

                    if (!indices.empty()) {
                        bd.ByteWidth = sizeof(UINT) * static_cast<UINT>(indices.size());
                        bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
                        initVB.pSysMem = indices.data();
                        device->CreateBuffer(&bd, &initVB, mesh.index_buffer.GetAddressOf());
                    }

                    mesh.vertex_count = static_cast<UINT>(vertices.size());
                    mesh.index_count = static_cast<UINT>(indices.size());

                    if (aimesh->mMaterialIndex < scene->mNumMaterials) {
                        aiMaterial* mat = scene->mMaterials[aimesh->mMaterialIndex];

                        aiString tex_path;
                        if (mat->GetTexture(aiTextureType_DIFFUSE, 0, &tex_path) == AI_SUCCESS) {
                            std::string full_path = tex_path.C_Str();
                            std::replace(full_path.begin(), full_path.end(), '\\', '/');
                            LoadTexture(render, device, full_path);

                            auto srv_source = GetTexture(std::filesystem::path(full_path).stem().string());
                            if (srv_source) {
                                mesh.diffuseSRV = srv_source->d3d_srv.Get();
                            }
                            else {
                                mesh.diffuseSRV = nullptr;
                            }
                        }
                        model->meshes.push_back(std::move(mesh));
                    }
                }
            }


               for (unsigned int i = 0; i < node->mNumChildren; ++i)
                    ProcessNode(node->mChildren[i], transform);
            };

            ProcessNode(scene->mRootNode, aiMatrix4x4());

            if (scene->HasCameras()) {
                for (unsigned int i = 0; i < scene->mNumCameras; ++i) {
                    aiCamera* cam = scene->mCameras[i];

                    // 查找对应的 aiNode
                    aiNode* camera_node = scene->mRootNode->FindNode(cam->mName);

                    // 初始化变量
                    aiVector3D world_position;
                    aiVector3D world_forward;
                    aiVector3D world_up;

                    if (camera_node) {
                        aiMatrix4x4 global_transform = get_global_transformation(camera_node);

                        world_position.Set(global_transform.a4, global_transform.b4, global_transform.c4);

                        world_forward = global_transform * cam->mLookAt;
                        world_up = global_transform * cam->mUp;
                    }
                    else {
                        std::cerr << "Warning: Could not find node for camera: " << cam->mName.C_Str() << ". Using local values." << std::endl;
                        world_position = cam->mPosition;
                        world_forward = cam->mLookAt;
                        world_up = cam->mUp;
                    }

                    aiVector3D target_position = world_position + world_forward;

                    float fov = cam->mHorizontalFOV;
                    float near_clip = cam->mClipPlaneNear;
                    float far_clip = cam->mClipPlaneFar;

                    model->cameras.emplace(cam->mName.C_Str(), Render::D3D::CameraData(
                        { world_position.x, world_position.y, world_position.z },    // 世界位置
                        { target_position.x, target_position.y, target_position.z }, // 目标世界位置
                        { world_up.x, world_up.y, world_up.z },                      // 摄像机上方向
                        fov, near_clip, far_clip));
                }
            }

            if (scene->HasLights()) {
                for (unsigned int i = 0; i < scene->mNumLights; ++i) {
                    aiLight* light = scene->mLights[i];

                    // 查找对应的 aiNode
                    aiNode* light_node = scene->mRootNode->FindNode(light->mName);

                    Render::D3D::Light light_data;
                    aiVector3D world_pos;
                    aiVector3D world_direction;

                    if (light_node) {
                        aiMatrix4x4 global_transform = get_global_transformation(light_node);
                        world_pos.Set(global_transform.a4, global_transform.b4, global_transform.c4);

                        world_direction = global_transform * light->mDirection;

                    }
                    else {
                        world_pos = light->mPosition;
                        world_direction = light->mDirection;
                    }

                    light_data.position = { world_pos.x, world_pos.y, world_pos.z };

                    // 归一化方向向量 (始终执行)
                    world_direction.Normalize();
                    light_data.direction = { world_direction.x, world_direction.y, world_direction.z };

                    light_data.color = { light->mColorDiffuse.r, light->mColorDiffuse.g, light->mColorDiffuse.b, 1.0f };
                    light_data.constant_attenuation = light->mAttenuationConstant;
                    light_data.linear_attenuation = light->mAttenuationLinear;
                    light_data.quadratic_attenuation = light->mAttenuationQuadratic;
                    light_data.inner_cos_angle = std::cos(light->mAngleInnerCone * 0.5f);
                    light_data.outer_cos_angle = std::cos(light->mAngleOuterCone * 0.5f);

                    model->lights.push_back(std::move(light_data));
                }
            }

            if (scene->HasAnimations()) {
                for (unsigned int i = 0; i < scene->mNumAnimations; ++i) {
                    aiAnimation* anim = scene->mAnimations[i];

                    Render::D3D::AnimationClip ac;

                    ac.duration = anim->mTicksPerSecond != 0 ?
                        static_cast<float>(anim->mDuration / anim->mTicksPerSecond) :
                        static_cast<float>(anim->mDuration / 30.0f);

                    for (unsigned int j = 0; j < anim->mNumChannels; ++j) {
                        aiNodeAnim* node_anim = anim->mChannels[j];
                        std::string node_name = node_anim->mNodeName.C_Str();

                        std::vector<Render::D3D::AnimationKeyFrame> kf;

                        unsigned int max_keys_count = std::max({
                            node_anim->mNumPositionKeys,
                            node_anim->mNumRotationKeys,
                            node_anim->mNumScalingKeys
                            });
                        kf.reserve(max_keys_count);

                        unsigned int num_pos_keys = node_anim->mNumPositionKeys;

                        for (unsigned int k = 0; k < num_pos_keys; ++k) {
                            if (k >= node_anim->mNumRotationKeys || k >= node_anim->mNumScalingKeys) {
                                break;
                            }

                            Render::D3D::AnimationKeyFrame current_key_frame;
                            current_key_frame.time = static_cast<float>(node_anim->mPositionKeys[k].mTime);

                            current_key_frame.position = {
                                node_anim->mPositionKeys[k].mValue.x,
                                node_anim->mPositionKeys[k].mValue.y,
                                node_anim->mPositionKeys[k].mValue.z,
                            };

                            current_key_frame.rotation = {
                                node_anim->mRotationKeys[k].mValue.x,
                                node_anim->mRotationKeys[k].mValue.y,
                                node_anim->mRotationKeys[k].mValue.z,
                                node_anim->mRotationKeys[k].mValue.w
                            };
                            current_key_frame.scale = {
                                node_anim->mScalingKeys[k].mValue.x,
                                node_anim->mScalingKeys[k].mValue.y,
                                node_anim->mScalingKeys[k].mValue.z
                            };

                            kf.push_back(std::move(current_key_frame));
                        }
                        ac.node_channels.emplace(node_name, std::move(kf));
                    }
                    model->animations.emplace(anim->mName.C_Str(), std::move(ac));
                }
            }

            models[key] = model;
            spdlog::info("成功加载3D模型: {} ({} meshes)", key, model->meshes.size());
            return model;
    }
}

