#include "camera.h"
#include "D3D/model.h"
#include <cmath>
namespace Engine::Render {
	Camera::Camera(): 
		m_position(XMVectorSet(0, 0, -10, 1)), m_target(XMVectorSet(0, 0, 0, 1)),
		m_up(XMVectorSet(0, 1, 0, 0)), pitch(0.0f), yaw(0.0f),
		fov(XM_PIDIV4), aspect_ratio(16.0f / 9.0f), near_z(0.1f), far_z(1000.0f),
		m_view(XMMatrixIdentity()), m_proj(XMMatrixIdentity())
	{
		UpdateView();
		UpdateProj();
	}

	Camera::Camera(const XMVECTOR& _pos, const XMVECTOR& _tar, const XMVECTOR& _up,
		float _fov, float _aspect, float _near_z, float _far_z) :
		m_position(_pos), m_target(_tar), m_up(_up),
		pitch(0.0f), yaw(0.0f), fov(_fov),
		aspect_ratio(_aspect), near_z(_near_z), far_z(_far_z),
		m_view(XMMatrixIdentity()), m_proj(XMMatrixIdentity())
	{
		UpdateProj();
		m_view = XMMatrixLookAtLH(m_position, m_target, m_up);
	}

	Camera::Camera(const D3D::CameraData& _cd, float _aspect)
		:fov(_cd.fov), aspect_ratio(_aspect), near_z(_cd.near_plane),
		far_z(_cd.far_plane), pitch(0.0f), yaw(0.0f)
	{
		m_position = { _cd.position.x, _cd.position.y, _cd.position.z };
		m_target = { _cd.target.x, _cd.target.y, _cd.target.z };
		m_up = { _cd.up.x, _cd.up.y, _cd.up.z };
		UpdateProj();
		m_view = XMMatrixLookAtLH(m_position, m_target, m_up);
	}

	void Camera::UpdateAspect(float _aspect)
	{
		aspect_ratio = _aspect;
	}

	void Camera::UpdateView()
	{
		XMMATRIX rotation_matrix = XMMatrixRotationRollPitchYaw(pitch, yaw, 0.0f);

		XMVECTOR default_forward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
		XMVECTOR look_direction = XMVector3TransformCoord(default_forward, rotation_matrix);
		m_target = m_position + look_direction;
		m_view = XMMatrixLookAtLH(m_position, m_target, m_up);
	}

	void Camera::UpdateProj()
	{
		m_proj = XMMatrixPerspectiveFovLH(fov, aspect_ratio, near_z, far_z);
	}

	void Camera::SetPosition(const XMVECTOR& _pos)
	{
		m_position = _pos;
		UpdateView();
	}

	void Camera::SmoothMove(const XMVECTOR& _target_position, float _smoothness, float delta_time) {
		XMVECTOR current_pos = m_position;
		XMVECTOR new_pos = XMVectorLerp(current_pos, _target_position,
			1.0f - exp(-_smoothness * delta_time));
		SetPosition(new_pos);
	}

	void Camera::SetRotation(float _pitch, float _yaw)
	{
		_pitch = std::clamp(_pitch, -XM_PIDIV2 * 0.9f, XM_PIDIV2 * 0.9f);
		yaw = std::fmod(_yaw, XM_2PI);
		pitch = _pitch;
		yaw = _yaw;

		UpdateView();
	}

	void Camera::SetFog(float _start, float _end, const XMFLOAT3& _color, bool _enabled) {
		fog.fog_start = _start;
		fog.fog_end = _end;
		fog.fog_color = _color;
		fog.fog_enable = _enabled ? XMFLOAT3{ 1.0f, 0.0f, 0.0f } : XMFLOAT3{ 0.0f, 0.0f, 0.0f };
	}

	void Camera::Move(const XMVECTOR& _world_space_move_vector, float _speed)
	{
		XMVECTOR displacement = _world_space_move_vector * _speed;
		m_position = m_position + displacement;
		UpdateView();
	}

	static XMVECTOR LoadFloat3(const XMFLOAT3& f3) {
		return XMLoadFloat3(&f3);
	}

	void Camera::SetPositionFloat3(const XMFLOAT3& pos) {
		m_position = LoadFloat3(pos);
		UpdateView();
	}

	void Camera::SetTargetFloat3(const XMFLOAT3& tar) {
		m_target = LoadFloat3(tar);
		XMVECTOR dir = m_target - m_position;
		dir = XMVector3Normalize(dir);
		XMFLOAT3 d;
		XMStoreFloat3(&d, dir);
		pitch = asin(std::clamp(d.y, -0.999f, 0.999f));
		if (abs(d.x) > 0.001f || abs(d.z) > 0.001f) {
			yaw = atan2(d.x, d.z);
		}
		UpdateView();
	}

	void Camera::SetUpFloat3(const XMFLOAT3& up) {
		m_up = LoadFloat3(up);
		UpdateView();
	}

	void Camera::SetFov(float _fov) {
		fov = _fov;
		UpdateProj();
	}

	void Camera::SetAspect(float aspect)
	{
		aspect_ratio = aspect;
		UpdateProj();
	}

	void Camera::SetNearFarZ(float _near_z, float _far_z) {
		near_z = _near_z;
		far_z = _far_z;
		UpdateProj();
	}

	XMFLOAT4 Camera::GetPositionFloat4() const {
		XMFLOAT4 f4;
		XMStoreFloat4(&f4, m_position);
		f4.w = 1.0f;
		return f4;
	}
}