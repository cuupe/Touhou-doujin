#pragma once
#include "../prefix.h"
namespace Engine::Core {
	class Context;
	class GameObject;
}
using namespace Engine::Core;
namespace Engine::Render {
	class Renderer;
}
using namespace Engine::Render;
namespace Engine::Scene {
	class SceneManager;

	class Scene {
	protected:
		std::string scene_name;
		Context& ctx;
		SceneManager& scene_manager;
		bool is_init = false;
		std::vector<std::unique_ptr<GameObject>> game_objects;
		std::vector<std::unique_ptr<GameObject>> pending;

	public:
		Scene(const std::string& s_n, Context& c, SceneManager& s_m);
		virtual ~Scene();
		Scene(const Scene&) = delete;
		Scene& operator=(const Scene&) = delete;
		Scene(Scene&&) = delete;
		Scene& operator=(Scene&&) = delete;

	public:
		virtual void Init();
		virtual void Update(float);
		virtual void Render();
		virtual void HandleInput();
		virtual void Destroy();

	public:
		virtual void AddGameObject(std::unique_ptr<GameObject>&& g_o);
		virtual void AddGameObjectSafe(std::unique_ptr<GameObject>&& g_o);
		virtual void RemoveGameObject(GameObject* g_o);
		virtual void RemoveGameObjectSafe(GameObject* g_o);
		const std::vector<std::unique_ptr<GameObject>>& GetGameObjects() const;
		GameObject* FindGameObject(const std::string& name) const;

	public:
		const std::string& GetName() const { return scene_name; }
		void SetName(const std::string& n_n) { scene_name = n_n; }
		void SetInit(bool init) { is_init = init; }
		bool IsInit() const { return is_init; }

		Context& GetContext() const { return ctx; }
		SceneManager& GetSceneMannager() const { return scene_manager; }
		std::vector<std::unique_ptr<GameObject>>& GetGameObjects() { return game_objects; }

	protected:
		void ProcessPending();
	};

}
