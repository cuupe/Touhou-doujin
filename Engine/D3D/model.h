#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <wrl/client.h>
#include <unordered_map>
#include <vector>
#include <string>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib") 
namespace Engine::Render::D3D {
    constexpr int MAX_LIGHTS_SIZE = 16;
    using namespace DirectX;
    using Microsoft::WRL::ComPtr;
    struct Vertex {
        XMFLOAT3 pos;
        XMFLOAT2 tex;
        XMFLOAT4 color;
    };

    struct Vertex1 {
        XMFLOAT3 pos;
        XMFLOAT3 norm;
        XMFLOAT2 tex;
        XMFLOAT4 color;
    };

    struct Mesh {
        ComPtr<ID3D11Buffer> vertex_buffer;
        ComPtr<ID3D11Buffer> index_buffer;
        UINT vertex_count = 0;
        UINT index_count = 0;
        DXGI_FORMAT index_format = DXGI_FORMAT_R32_UINT;
        
        ComPtr<ID3D11ShaderResourceView> diffuseSRV;
        ComPtr<ID3D11ShaderResourceView> diffuseSRV_a;
        XMFLOAT4 base_color = { 1.0f,1.0f,1.0f,1.0f };
    };

    struct CameraData {
        XMFLOAT3 position;
        XMFLOAT3 target;
        XMFLOAT3 up;

        float fov = XM_PIDIV4;
        float near_plane = 0.01f;
        float far_plane = 1e3f;
        CameraData(const XMFLOAT3& pos,
            const XMFLOAT3& tar,
            const XMFLOAT3& _up):
            position(pos), target(tar),
            up(_up){ }
        CameraData(const XMFLOAT3& pos,
            const XMFLOAT3& tar,
            const XMFLOAT3& _up,
            float f, float n_p, float f_p)
            : position(pos), target(tar),
            up(_up), fov(f), near_plane(n_p),
            far_plane(f_p){ }


        XMMATRIX GetViewMatrix() const {
            return XMMatrixLookAtLH(
                XMLoadFloat3(&position),
                XMLoadFloat3(&target),
                XMLoadFloat3(&up)
            );
        }

        XMMATRIX GetProjectionMatrix(float aspect) const {
            float vertical_fov = 2.0f * std::atan(std::tan(fov * 0.5f) / aspect);
            return XMMatrixPerspectiveFovLH(vertical_fov, aspect, near_plane, far_plane);
        }
    };

    enum class LightType {
        DIRECTIONAL_LIGHT = 0,
        POINT_LIGHT = 1,
        SPOT_LIGHT = 2
    };

    struct Light {
        XMFLOAT4 color;
        XMFLOAT3 position;
        LightType lightType;
        XMFLOAT3 direction;
        float constant_attenuation;
        float linear_attenuation;
        float quadratic_attenuation;
        float inner_cos_angle;
        float outer_cos_angle;

        Light(
            const XMFLOAT4& _color = { 1.0f, 1.0f, 1.0f, 1.0f },
            const XMFLOAT3& _position = { 0.0f, 0.0f, 0.0f },
            LightType _lightType = LightType::DIRECTIONAL_LIGHT,
            const XMFLOAT3& _direction = { 0.0f, -1.0f, 0.0f },
            float _constantAtt = 1.0f,
            float _linearAtt = 0.0f,
            float _quadratic_attenuation = 0.0f,
            float _innerConeDeg = 360.0f,
            float _outerConeDeg = 360.0f
        ) :
            color(_color),
            position(_position),
            lightType(_lightType),
            direction(_direction),
            constant_attenuation(_constantAtt),
            linear_attenuation(_linearAtt),
            quadratic_attenuation(_quadratic_attenuation)
        {
            if (_lightType == LightType::SPOT_LIGHT) {
                inner_cos_angle = std::cos(XMConvertToRadians(_innerConeDeg));
                outer_cos_angle = std::cos(XMConvertToRadians(_outerConeDeg));
            }
            else {
                inner_cos_angle = -1.0f;
                outer_cos_angle = -1.0f;
            }

            // 规范化方向向量
            XMVECTOR dir_vec = XMLoadFloat3(&this->direction);
            dir_vec = XMVector3Normalize(dir_vec);
            XMStoreFloat3(&this->direction, dir_vec);
        }
    };

    struct Fog {
        float fog_start;
        float fog_end;
        XMFLOAT3 fog_color = { 0.5f, 0.5f, 0.5f };
        XMFLOAT3 fog_enable = { 0.0f, 0.0f, 0.0f };
    };

    struct AnimationKeyFrame {
        float time;
        XMFLOAT3 position;
        XMFLOAT4 rotation;
        XMFLOAT3 scale;
        AnimationKeyFrame(
            float t = 0.0f,
            const XMFLOAT3& pos = { 0.0f, 0.0f, 0.0f },
            const XMFLOAT4& rot = { 0.0f, 0.0f, 0.0f, 1.0f },
            const XMFLOAT3& s = { 1.0f, 1.0f, 1.0f }
        ) :time(t), position(pos), rotation(rot), scale(s) { }
    };

    struct AnimationClip {
        float duration;
        std::unordered_map<std::string, std::vector<AnimationKeyFrame>> node_channels;

        AnimationClip(float total_duration = 0.0f) 
            : duration(total_duration)
        { }
    };

    struct ModelResource {
        std::string name;
        XMMATRIX world_transform = XMMatrixIdentity();
        std::vector<Mesh> meshes;
        std::vector<Light> lights;
        std::unordered_map<std::string, CameraData> cameras;
        std::unordered_map<std::string, AnimationClip> animations;
    };

    //常量缓冲区必须按16字节对齐
    struct ConstantBuffer {
        XMFLOAT4X4 world_view_proj;
    };

    struct CBPerFrame {
        XMMATRIX m_view;
        XMMATRIX m_projection;
        XMFLOAT4 camera_position; //包含padding
        XMFLOAT4 ambient_color;
    };

    struct CBPerObject {
        XMMATRIX m_world;
        XMFLOAT4 mesh_color;
        XMFLOAT4 mix;   //包含padding
    };

    
    struct CBLight {
        Light lights[MAX_LIGHTS_SIZE];
        XMFLOAT4 light_count; //包含padding
    };

    struct CBFog {
        float fog_start = 10.0f;
        XMFLOAT3 fog_color = { 0.5f, 0.5f, 0.5f };
        float fog_end = 100.0f;
        XMFLOAT3 fog_enable = { 0.0f, 0.0f, 0.0f };
    };
}