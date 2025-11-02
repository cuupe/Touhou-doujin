#pragma once
#include "../prefix.h"
namespace Engine::Core {
	class Context;
}
using namespace Engine::Core;
namespace Engine::Scene {
	class Scene;
}
using namespace Engine::Core;

namespace Engine::Scene {
	class SceneManager final{
	private:
		Context& ctx;
		std::vector<std::unique_ptr<Scene>> scene_stack;
		enum class PendingAction {NONE, PUSH, POP, REPLACE};
		PendingAction pend_act = PendingAction::NONE;
		std::unique_ptr<Scene> pend_scene;

	public:
		explicit SceneManager(Context& context);
		~SceneManager();
		SceneManager(const SceneManager&) = delete;
		SceneManager& operator=(const SceneManager&) = delete;
		SceneManager(SceneManager&&) = delete;
		SceneManager& operator=(SceneManager&&) = delete;

	public:
		Scene* GetCurrentScene() const;
		Context& GetContext() const { return ctx; }

	public:
		void RequestPushScene(std::unique_ptr<Scene>&& scene);
		void RequestPopScene();
		void RequestReplaceScene(std::unique_ptr<Scene>&& scene);


	public:
		void Update(float);
		void Render();
		void HandleInput();
		void Close();

	private:
		void ProcessPendingActions();
		void PushScene(std::unique_ptr<Scene>&& scene);
		void PopScene();
		void ReplaceScene(std::unique_ptr<Scene>&& scene);

	};
}