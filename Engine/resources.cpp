#include "resources.h"
#include "D3D/model.h"
#ifdef max
#undef max
#endif
namespace Engine::Resource {
    ResourceManager::ResourceManager(ID3D11Device* device)
    {
        unsigned int white = 0xFFFFFFFF;
        D3D11_SUBRESOURCE_DATA init = { &white, 4, 0 };
        CD3D11_TEXTURE2D_DESC desc(DXGI_FORMAT_R8G8B8A8_UNORM, 1, 1);
        ComPtr<ID3D11Texture2D> tex;
        device->CreateTexture2D(&desc, &init, tex.GetAddressOf());
        device->CreateShaderResourceView(tex.Get(), nullptr, default_srv.GetAddressOf());
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

        // 自动推导格式：如果纹理本身有 typed 格式，就直接用；否则手动转换
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

        texs.insert({ filename, std::move(tex_res)});
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


    void ResourceManager::LoadFont(const std::string& font_name, const std::string& path, int size)
    {
        TTF_Font* font = TTF_OpenFont(path.c_str(), size);
        if (font == nullptr) {
            spdlog::error("无法加载字体文件：{} ,ERROR:{}", path, SDL_GetError());
            throw std::runtime_error("发生错误");
        }
        std::unique_ptr<FontResource> ft = std::make_unique<FontResource>();
        ft->font = FontPtr(font);
        fonts.insert({ font_name, std::move(ft) });
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
            aiProcess_FlipUVs |
            aiProcess_JoinIdenticalVertices |
            aiProcess_MakeLeftHanded |
            aiProcess_FlipWindingOrder
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
            [&](aiNode* node, const aiMatrix4x4& parentTransform){
                aiMatrix4x4 transform = parentTransform * node->mTransformation;
                if (scene->HasMeshes()) {
                    for (unsigned int i = 0; i < node->mNumMeshes; ++i)
                    {
                        aiMesh* aimesh = scene->mMeshes[node->mMeshes[i]];
                        Render::D3D::Mesh mesh;

                        DirectX::XMMATRIX nodeTransform = *reinterpret_cast<DirectX::XMMATRIX*>(&transform);
                        //顶点
                        std::vector<Render::D3D::Vertex> vertices;
                        vertices.reserve(aimesh->mNumVertices);
                        for (unsigned int v = 0; v < aimesh->mNumVertices; ++v)
                        {
                            Render::D3D::Vertex vert;

                            vert.Pos = DirectX::XMFLOAT3(aimesh->mVertices[v].x, aimesh->mVertices[v].y, aimesh->mVertices[v].z);
                            vert.Tex = aimesh->HasTextureCoords(0) ?
                                DirectX::XMFLOAT2(aimesh->mTextureCoords[0][v].x, aimesh->mTextureCoords[0][v].y) :
                                DirectX::XMFLOAT2(0, 0);
                            vert.Color = aimesh->HasVertexColors(0) ?
                                DirectX::XMFLOAT4(aimesh->mColors[0][v].r, aimesh->mColors[0][v].g,
                                    aimesh->mColors[0][v].b, aimesh->mColors[0][v].a) :
                                DirectX::XMFLOAT4(1, 1, 1, 1);
                            vertices.push_back(std::move(vert));
                        }

                        std::vector<UINT> indices;
                        for (unsigned int f = 0; f < aimesh->mNumFaces; ++f) {
                            for (unsigned int k = 0; k < aimesh->mFaces[f].mNumIndices; ++k) {
                                indices.emplace_back(aimesh->mFaces[f].mIndices[k]);
                            }
                        }

                        D3D11_BUFFER_DESC bd = { sizeof(Render::D3D::Vertex) * (UINT)vertices.size(),
                            D3D11_USAGE_DEFAULT, D3D11_BIND_VERTEX_BUFFER };
                        D3D11_SUBRESOURCE_DATA initVB = { vertices.data() };
                        device->CreateBuffer(&bd, &initVB, mesh.vertex_buffer.GetAddressOf());

                        if (!indices.empty()) {
                            bd.ByteWidth = sizeof(UINT) * (UINT)indices.size();
                            bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
                            initVB.pSysMem = indices.data();
                            device->CreateBuffer(&bd, &initVB, mesh.index_buffer.GetAddressOf());
                        }

                        mesh.vertex_count = (UINT)vertices.size();
                        mesh.index_count = (UINT)indices.size();

                        if (aimesh->mMaterialIndex < scene->mNumMaterials) {
                            aiMaterial* mat = scene->mMaterials[aimesh->mMaterialIndex];

                            aiString tex_path;
                            if (mat->GetTexture(aiTextureType_DIFFUSE, 0, &tex_path) == AI_SUCCESS) {
                                std::string full_path = dir + tex_path.C_Str();
                                std::replace(full_path.begin(), full_path.end(), '\\', '/');
                                LoadTexture(render, device, full_path);

                                auto srv_source = GetTexture(tex_path.C_Str());
                                if (srv_source) {
                                    mesh.diffuseSRV = srv_source->d3d_srv.Get();
                                }
                                else {
                                    mesh.diffuseSRV = nullptr;
                                }
                            }
                            else {
                                aiString mat_name;
                                mat->Get(AI_MATKEY_NAME, mat_name);
                                std::string utf8_name(mat_name.C_Str());
                                std::string full_path = dir + utf8_name + ".png";
                                std::replace(full_path.begin(), full_path.end(), '\\', '/');
                                spdlog::info("{}", full_path);
                                {

                                    //写死的暂时，中文有问题
                                    LoadTexture(render, device, "C:/Users/Lenovo/Desktop/srd/clo2.png");

                                    auto srv_source = GetTexture("clo2");
                                    mesh.diffuseSRV = srv_source ? srv_source->d3d_srv : default_srv;
                                }
                            }
                        }


                        model->meshes.push_back(std::move(mesh));
                    }
                }

                

                for (unsigned int i = 0; i < node->mNumChildren; ++i)
                    ProcessNode(node->mChildren[i], transform);
            };

        ProcessNode(scene->mRootNode, aiMatrix4x4());

        if (scene->HasCameras()) {
            for (unsigned int i = 0; i < scene->mNumCameras; ++i) {
                aiCamera* cam = scene->mCameras[i];

                std::string _name = cam->mName.C_Str();
                aiVector3D _pos = cam->mPosition;
                aiVector3D _target = cam->mLookAt;
                aiVector3D _up = cam->mUp;

                float _fov = cam->mHorizontalFOV;
                float _near = cam->mClipPlaneNear;
                float _far = cam->mClipPlaneFar;

                model->cameras.insert({ _name, std::move(Render::D3D::Camera(
                    {_pos.x, _pos.y, _pos.z},
                    {_target.x, _target.y, _target.z},
                    {_up.x, _up.y, _up.z},
                    _fov, _near, _far)) });
            }
        }

        if (scene->HasLights()) {
            for (unsigned int i = 0; i < scene->mNumLights; ++i) {
                aiLight* light = scene->mLights[i];

                Render::D3D::Light m_light;
                m_light.color = {
                    light->mColorDiffuse.r,
                    light->mColorDiffuse.g,
                    light->mColorDiffuse.b,
                    1.0f
                };
                m_light.position = {
                    light->mPosition.x,
                    light->mPosition.y,
                    light->mPosition.z
                };
                m_light.direction = {
                    light->mDirection.x,
                    light->mDirection.y,
                    light->mDirection.z
                };

                m_light.constant_attenuation = light->mAttenuationConstant;
                m_light.linear_attenuation = light->mAttenuationLinear;
                m_light.quadratic_attenuation = light->mAttenuationQuadratic;

                m_light.inner_cos_angle = light->mAngleInnerCone;
                m_light.outer_cos_angle = light->mAngleOuterCone;

                model->lights.push_back(std::move(m_light));
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
                        if (k >= node_anim->mNumPositionKeys || k >= node_anim->mNumScalingKeys) {
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
};

