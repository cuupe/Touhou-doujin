#include "Loading.h"
#include "../Engine/render.h"
#include "../Engine/schedule.h"
#include "../Engine/context.h"
#include "../Engine/EffectManager.h"
#include "../Engine/text.h"
#include "TitleScene.h"
#include "../../Engine/Components/TransformComponent.h"
#include "../../Engine/Components/SpriteComponent.h"
#include <nlohmann/json.hpp>

namespace Game::Scene {
	Loading::Loading(const std::string& name, Engine::Core::Context& ctx,
		Engine::Scene::SceneManager& sm, Engine::Audio::AudioManager& _au, 
		Engine::UISystem::UIManager& _ui, Engine::Render::TextManager& _tm)
		: Engine::Scene::Scene(name, ctx, sm, _au, _ui)
	{ }

	void Loading::Init()
	{
		auto& res = ctx.GetResourceManager();
		auto device = ctx.GetDevice();
		auto& render = ctx.GetRenderer();

		std::ifstream file("data/resource.json");
		nlohmann::json json;
		file >> json;
		for (const auto& resource : json["base"]) {
			res.LoadTexture(render.GetSDLRenderer(), device, resource["path"].get<std::string>());
			std::unique_ptr<Engine::Core::GameObject> gb = std::make_unique<Engine::Core::GameObject>();
			gb->AddComponent<Engine::Core::Components::TransformComponent>(
				Engine::Maths::Vec2{resource["transform"]["x"], resource["transform"]["y"] });
			gb->AddComponent<Engine::Core::Components::SpriteComponent>(resource["name"], res,
				SDL_FRect{resource["sprite"]["x"],resource["sprite"]["y"],
				resource["sprite"]["w"], resource["sprite"]["h"] }
			);
			if (resource.contains("anim")) {
				Engine::Render::Effect::EffectManager::Blink(gb.get(), resource["anim"]["freq"], resource["anim"]["min_alpha"]);
			}
			AddGameObject(std::move(gb));
		}
		for (const auto& resource : json["resources"]) {
			tasks.emplace(
				resource["type"].get<std::string>(),
				resource["path"].get<std::string>()
			);
			++total_tasks;
		}
		auto ft = ui.GetTextManager().GetFTLib();
		loading_thread = std::thread([this, &res, &render, device, ft]() {
			ResourceLoadInfo task;
			auto r = render.GetSDLRenderer();
			while (!request_stop) {
				{
					std::lock_guard<std::mutex> lock(tasks_mutex);
					if (tasks.empty()) {
						break;
					}
					task = tasks.front();
					tasks.pop();
				}

				if (task.type == "texture") {
					res.LoadTexture(r, device, task.path);
				}
				else if (task.type == "audio") {
					res.LoadAudio(au.GetMixer(), task.path);
				}
				else if (task.type == "model") {
					res.LoadModel(r, device, task.path);
				}
				else if (task.type == "font") {
					res.LoadFont(ctx.GetDevice(), ft, task.path, 30);
				}
				++completed_tasks;
			}
			});

		is_init = true;
	}

	void Loading::Update(float d_t)
	{
		if (completed_tasks >= total_tasks) {
			std::lock_guard<std::mutex> lock(tasks_mutex);

			if (tasks.empty()) {
				if (loading_thread.joinable()) {
					loading_thread.join();
				}

				on_leaving = true;
			}
		}

		if (on_leaving) {
			alpha -= 3;
			if (alpha == 0) {
				scene_manager.RequestReplaceScene(std::make_unique<TitleScene>("title", ctx, scene_manager, au, ui));
			}
			for (auto& obj : game_objects) {
				auto sp = obj->GetComponent<Engine::Core::Components::SpriteComponent>();
				sp->SetColorMod(SDL_Color{ 255, 255, 255, alpha });
			}
		}


		Scene::Update(d_t);
	}

	void Loading::Render()
	{
		auto& r = ctx.GetRenderer();
		r.Begin2D();
		Scene::Render();
		for (auto& obj : game_objects) {
			auto sp = obj->GetComponent<Engine::Core::Components::SpriteComponent>();
			sp->SetColorMod(SDL_Color{ 255, 255, 255, 255 });
		}
	}
}