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
    using namespace DirectX;
    using Microsoft::WRL::ComPtr;
    struct Vertex {
        XMFLOAT3 Pos;
        XMFLOAT2 Tex;
        XMFLOAT4 Color;
    };

    struct Mesh {
        ComPtr<ID3D11Buffer> vertex_buffer;
        ComPtr<ID3D11Buffer> index_buffer;
        UINT vertex_count = 0;
        UINT index_count = 0;
        DXGI_FORMAT index_format = DXGI_FORMAT_R32_UINT;
        
        ComPtr<ID3D11ShaderResourceView> diffuseSRV;
        XMFLOAT4 base_color = { 1.0f,1.0f,1.0f,1.0f };
    };

    struct Camera {
        XMFLOAT3 position;
        XMFLOAT3 target;
        XMFLOAT3 up;

        float fov = XM_PIDIV4;
        float near_plane = 0.01f;
        float far_plane = 1e3;

        Camera(const XMFLOAT3& pos,
            const XMFLOAT3& tar,
            const XMFLOAT3& _up):
            position(pos), target(tar),
            up(_up){ }
        Camera(const XMFLOAT3& pos,
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
            return XMMatrixPerspectiveFovLH(fov, aspect, near_plane, far_plane);
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
            // 颜色与强度
            const XMFLOAT4& _color = { 1.0f, 1.0f, 1.0f, 1.0f },
            // 位置与类型
            const XMFLOAT3& _position = { 0.0f, 0.0f, 0.0f },
            LightType _lightType = LightType::DIRECTIONAL_LIGHT,
            // 方向与衰减
            const XMFLOAT3& _direction = { 0.0f, -1.0f, 0.0f }, // 默认向下
            float _constantAtt = 1.0f,
            // 衰减与聚光灯角度 (默认设置为点光源/定向光常用的值)
            float _linearAtt = 0.0f,
            float _quadratic_attenuation = 0.0f,
            float _innerConeDeg = 360.0f, // 默认大于 90 度，使其行为像点光源
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
            // 角度转换为余弦值，用于着色器中的光照计算
            // 确保输入的角度不是用于三角函数，而是用于余弦比较
            if (_lightType == LightType::SPOT_LIGHT) {
                // 将角度转换为弧度，然后取余弦值
                inner_cos_angle = std::cos(XMConvertToRadians(_innerConeDeg));
                outer_cos_angle = std::cos(XMConvertToRadians(_outerConeDeg));
            }
            else {
                // 非聚光灯类型，将余弦值设置为允许所有光线通过 (cos(0) = 1.0)
                inner_cos_angle = -1.0f;
                outer_cos_angle = -1.0f;
            }

            // 规范化方向向量
            XMVECTOR dir_vec = XMLoadFloat3(&this->direction);
            dir_vec = XMVector3Normalize(dir_vec);
            XMStoreFloat3(&this->direction, dir_vec);
        }
    };

    struct AnimationKeyFrame {
        float time; // 时间戳 (Ticks)
        XMFLOAT3 position;
        XMFLOAT4 rotation; // 使用 XMFLOAT4 存储四元数
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
        std::vector<Mesh> meshes;
        std::vector<Light> lights;
        std::unordered_map<std::string, Camera> cameras;
        std::unordered_map<std::string, AnimationClip> animations;
    };


    //拓展光照
    struct ConstantBuffer {
        XMMATRIX m_world;
        XMMATRIX m_view;
        XMMATRIX m_projection;
    };
}
