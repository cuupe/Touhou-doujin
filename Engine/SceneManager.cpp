#include "SceneManager.h"
#include "scene.h"
#include "context.h"
namespace Engine::Scene {
	SceneManager::SceneManager(Context& context)
		:ctx(context)
	{
		spdlog::trace("场景管理器创建");
	}

	SceneManager::~SceneManager()
	{
		spdlog::trace("销毁场景");
		Close();
	}


	Scene* SceneManager::GetCurrentScene() const
	{
		if (scene_stack.empty()) {
			return nullptr;
		}

		return scene_stack.back().get();
	}

	void SceneManager::RequestPushScene(std::unique_ptr<Scene>&& scene)
	{
		pend_act = PendingAction::PUSH;
		pend_scene = std::move(scene);
	}

	void SceneManager::RequestPopScene()
	{
		pend_act = PendingAction::POP;
	}

	void SceneManager::RequestReplaceScene(std::unique_ptr<Scene>&& scene)
	{
		pend_act = PendingAction::REPLACE;
		pend_scene = std::move(scene);
	}

	void SceneManager::Update(float d_t) {
		Scene* current_scene = GetCurrentScene();
		if (current_scene) {
			current_scene->Update(d_t);
		}

		ProcessPendingActions();
	}

	void SceneManager::HandleInput() {
		Scene* current_scene = GetCurrentScene();
		if (current_scene) {
			current_scene->HandleInput();
		}
	}


	void SceneManager::Render() {
		for (const auto& scene : scene_stack) {
			if (scene) {
				scene->Render();
			}
		}
	}

	void SceneManager::Close() {
		spdlog::trace("正在关闭场景管理器并清理场景栈");

		while (!scene_stack.empty()) {
			if (scene_stack.back()) {
				spdlog::debug("正在清理场景{}", scene_stack.back()->GetName());
				scene_stack.back()->Destroy();
			}
			scene_stack.pop_back();
		}
	}

	void SceneManager::ProcessPendingActions()
	{
		if (pend_act == PendingAction::NONE) {
			return;
		}

		switch (pend_act) {
		case PendingAction::POP:
			PopScene();
			break;
		case PendingAction::PUSH:
			PushScene(std::move(pend_scene));
			break;
		case PendingAction::REPLACE:
			ReplaceScene(std::move(pend_scene));
			break;
		default:break;
		}

		pend_act = PendingAction::NONE;
	}

	void SceneManager::PushScene(std::unique_ptr<Scene>&& scene)
	{
		if (!scene) {
			spdlog::warn("尝试将空场景压入栈");
			return;
		}

		if (!scene->IsInit()) {
			scene->Init();
		}

		scene_stack.push_back(std::move(scene));
	}

	void SceneManager::PopScene()
	{
		if (scene_stack.empty()) {
			spdlog::warn("空场景，无法弹出");
			return;
		}

		if (scene_stack.back()) {
			scene_stack.back()->Destroy();
		}
		scene_stack.pop_back();
	}

	void SceneManager::ReplaceScene(std::unique_ptr<Scene>&& scene)
	{
		if (!scene) {
			spdlog::warn("尝试用空场景替换");
			return;
		}

		while (!scene_stack.empty()) {
			if (scene_stack.back()) {
				scene_stack.back()->Destroy();
			}
			scene_stack.pop_back();
		}

		if (!scene->IsInit()) {
			scene->Init();
		}
		scene_stack.push_back(std::move(scene));
	}
}