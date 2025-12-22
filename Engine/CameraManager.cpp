#include "CameraManager.h"

namespace Engine::Render {
	void CameraManager::AddCamera(const std::string& name, std::unique_ptr<Camera>&& camera)
	{
		try {
			if (!camera) {
				throw std::runtime_error("无效摄像机");
				return;
			}

			if (cameras.count(name)) {
				spdlog::error("重复名称的摄像机");
				return;
			}

			cameras.emplace(name, std::move(camera));

			if (active_camera == nullptr) {
				active_camera = cameras.at(name).get();
			}
		}
		catch (const std::exception& err) {
			spdlog::error("初始化摄像机管理器发生错误:{}", err.what());
			return;
		}
	}

	bool CameraManager::SetActiveCamera(const std::string& name)
	{
		Camera* cam = FindCamera(name);
		if (!cam) {
			return false;
		}

		active_camera = cam;
		return true;
	}

	Camera* CameraManager::FindCamera(const std::string& name) const
	{
		if (cameras.find(name) == cameras.end()) {
			spdlog::error("找不到:{} 这个摄像机", name);
			return nullptr;
		}

		return cameras.at(name).get();
	}

#define IF_ACTIVE_CAM_THEN(action) \
    Camera* cam = GetActiveCamera(); \
    if (cam) { action; }

	void CameraManager::UpdateActiveCameraPosition(const XMFLOAT3& pos) {
		IF_ACTIVE_CAM_THEN(cam->SetPositionFloat3(pos))
	}

	void CameraManager::UpdateActiveCameraTarget(const XMFLOAT3& target) {
		IF_ACTIVE_CAM_THEN(cam->SetTargetFloat3(target))
	}

	void CameraManager::UpdateActiveCameraUp(const XMFLOAT3& up) {
		IF_ACTIVE_CAM_THEN(cam->SetUpFloat3(up))
	}

	void CameraManager::UpdateActiveCameraFov(float fov) {
		IF_ACTIVE_CAM_THEN(cam->SetFov(fov))
	}

	void CameraManager::UpdateActiveCameraZ(float near_z, float far_z) {
		IF_ACTIVE_CAM_THEN(cam->SetNearFarZ(near_z, far_z))
	}

	void CameraManager::UpdateActiveCameraFog(float start, float end, const XMFLOAT3& color, bool enabled) {
		IF_ACTIVE_CAM_THEN(cam->SetFog(start, end, color, enabled))
	}

#undef IF_ACTIVE_CAM_THEN


}