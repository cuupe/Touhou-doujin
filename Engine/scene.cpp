#include "scene.h"
#include "GameObject.h"

namespace Engine::Scene {
	Scene::Scene(const std::string& s_n, Context& c, SceneManager& s_m)
		:scene_name(s_n), ctx(c), scene_manager(s_m)
	{
		spdlog::trace("场景构建完成");
	}
	Scene::~Scene() = default;

	void Scene::Init() {
		is_init = true;
		spdlog::trace("场景初始化完成");
	}

	void Scene::Update(float d_t) {
		if (!is_init) {
			return;
		}

		for (auto it = game_objects.begin(); it != game_objects.end();) {
			if (*it && !(*it)->IsNeedRemove()) {
				(*it)->Update(d_t, ctx);
				++it;
			}
			else {
				if (*it) {
					(*it)->Destroy();
				}
				it = game_objects.erase(it);
			}
		}

		ProcessPending();
	}

	void Scene::Render() {
		if (!is_init) {
			return;
		}

		for (const auto& obj : game_objects) {
			if (obj) {
				obj->Render(ctx);
			}
		}
	}

	void Scene::HandleInput() {
		if (!is_init) {
			return;
		}

		for (auto it = game_objects.begin(); it != game_objects.end();) {
			if (*it && !(*it)->IsNeedRemove()) {
				(*it)->HandleInput(ctx);
				++it;
			}
			else {
				if (*it) {
					(*it)->Destroy();
				}
				it = game_objects.erase(it);
			}
		}
	}

	void Scene::Destroy() {
		if (!is_init) {
			return;
		}

		for (const auto& obj : game_objects) {
			if (obj) {
				obj->Destroy();
			}
		}
		game_objects.clear();

		is_init = false;
		spdlog::trace("场景{}清理完成", scene_name);
	}

	void Scene::AddGameObject(std::unique_ptr<GameObject>&& g_o)
	{
		if (g_o) {
			game_objects.push_back(std::move(g_o));
		}
		else {
			spdlog::warn("尝试向场景{}中添加空对象", scene_name);
		}
	}

	void Scene::AddGameObjectSafe(std::unique_ptr<GameObject>&& g_o)
	{
		if (g_o) {
			pending.push_back(std::move(g_o));
		}
		else {
			spdlog::warn("尝试向场景{}中添加空对象", scene_name);
		}
	}

	void Scene::RemoveGameObject(GameObject* g_o)
	{
		if (!g_o) {
			spdlog::warn("场景{}调用了空对象指针进行移除", scene_name);
			return;
		}
		//remove_if将符合条件的对象移至末尾
		auto it = std::remove_if(game_objects.begin(), game_objects.end(),
			[g_o](const std::unique_ptr<GameObject>& p) {
				return p.get() == g_o;
			});
		//只要存在，就删除所有符合条件
		if (it != game_objects.end()) {
			(*it)->Destroy();
			game_objects.erase(it, game_objects.end());
			spdlog::trace("场景{}成功移除对象", scene_name);
		}
		else {
			spdlog::warn("场景{}中未找到对象", scene_name);
		}
	}

	void Scene::RemoveGameObjectSafe(GameObject* g_o)
	{
		g_o->SetNeedRemove(true);
	}

	const std::vector<std::unique_ptr<GameObject>>& Scene::GetGameObjects() const
	{
		return game_objects;
	}

	GameObject* Scene::FindGameObject(const std::string& name) const
	{
		for (const auto& obj : game_objects) {
			if (obj && obj->GetName() == name) {
				return obj.get();
			}
		}
		return nullptr;
	}

	void Scene::ProcessPending()
	{
		for (auto& obj : pending) {
			AddGameObject(std::move(obj));
		}
		pending.clear();
	}
}