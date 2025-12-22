#pragma once;
#include "camera.h"
namespace Engine::Render {
	class CameraManager final {
	private:
		std::unordered_map<std::string, std::unique_ptr<Camera>> cameras;
		Camera* active_camera = nullptr;
		XMFLOAT4 ambient_color = { 0.8f, 0.8f, 0.8f, 1.0f };
	public:
		CameraManager() = default;
		CameraManager(const CameraManager&) = delete;
		CameraManager(CameraManager&&) = delete;
		CameraManager& operator=(const CameraManager&) = delete;
		CameraManager& operator=(CameraManager&&) = delete;
		~CameraManager() = default;

	public:
		void AddCamera(const std::string& name, std::unique_ptr<Camera>&& camera);
		bool SetActiveCamera(const std::string& name);
		Camera* GetActiveCamera() const { return active_camera; }
		Camera* FindCamera(const std::string& name) const;
		const XMFLOAT4& GetAmbientColor() const { return ambient_color; }
		void SetAmbientColor(const XMFLOAT4& color) { ambient_color = color; }

	public:
		void UpdateActiveCameraPosition(const XMFLOAT3& pos);
		void UpdateActiveCameraTarget(const XMFLOAT3& target);
		void UpdateActiveCameraUp(const XMFLOAT3& up);
		void UpdateActiveCameraFov(float fov);
		void UpdateActiveCameraZ(float near_z, float far_z);
		void UpdateActiveCameraFog(float start, float end, const XMFLOAT3& color, bool enabled);
	};
}