#pragma once
#include "../prefix.h"
#include "D3D/model.h"
namespace Engine::Render {
	using namespace DirectX;
	class Camera final {
	private:
		D3D::Fog fog;
		XMVECTOR m_position;
		XMVECTOR m_target;
		XMVECTOR m_up;
		float pitch;
		float yaw;
		float fov;
		float aspect_ratio;
		float near_z;
		float far_z;
		XMMATRIX m_view;
		XMMATRIX m_proj;

	public:
		Camera();
		Camera(const XMVECTOR& _pos, const XMVECTOR& _tar, const XMVECTOR& _up,
			float _fov, float _aspect, float _near_z, float _far_z);
		Camera(const D3D::CameraData& _cd, float _aspect);
		
	public:
		void UpdateAspect(float _aspect);
		void UpdateView();
		void UpdateProj();
		void SetPosition(const XMVECTOR& _pos);
		void SetRotation(float _pitch, float _yaw);
		void SetFog(float _start, float _end, const XMFLOAT3& _color, bool _enabled);
		void SmoothMove(const XMVECTOR& _target_position, float _smoothness, float delta_time);
		const D3D::Fog& GetFog() const { return fog; }
		void Move(const XMVECTOR& _world_space_move_vector, float _speed);
		XMVECTOR GetPosition() const { return m_position; }
		XMMATRIX GetViewMatrix() const { return m_view; }
		XMMATRIX GetProjectionMatrix() const { return m_proj; }
		XMFLOAT4 GetPositionFloat4() const;
		void SetPositionFloat3(const XMFLOAT3& pos);
		void SetTargetFloat3(const XMFLOAT3& tar);
		void SetUpFloat3(const XMFLOAT3& up);
		void SetFov(float _fov);
		void SetAspect(float aspect);
		void SetNearFarZ(float _near_z, float _far_z);
	};
}