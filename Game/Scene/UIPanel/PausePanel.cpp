#include "PausePanel.h"
#include "../../../Engine/InputManager.h"
#include "../../../Engine/AudioManager.h"
#include "../../../Engine/schedule.h"
#include "../../../Engine/UIManager.h"
#include "../../../Engine/EffectManager.h"
#include "../../../Engine/Components/TransformComponent.h"
#include "../../../Engine/Components/SpriteComponent.h"
#include "../../../Engine/Components/AnimationComponent.h"
#include "../TitleScene.h"
#include "../Stage.h"
#include <nlohmann/json.hpp>

namespace Game::UI {
	void PausePanel::Init(Engine::Core::Context& ctx)
	{
		try {
			std::ifstream ascii_file("data/pause_panel.json");
			nlohmann::json json;
			ascii_file >> json;

			std::vector<std::vector<std::unique_ptr<Engine::Core::GameObject>>> vec1(3);
			std::vector<std::vector<std::unique_ptr<Engine::Core::GameObject>>> vec2(3);
			for (const auto& c : json["pause_panel"]) {
				std::unique_ptr<Engine::Core::GameObject> gb =
					std::make_unique<Engine::Core::GameObject>(c["id"].get<std::string>());
				gb->AddComponent<Engine::Core::Components::TransformComponent>(
					Engine::Maths::Vec2{ c["position"][0], c["position"][1] },
					Engine::Maths::Vec2{ c["scale"][0], c["scale"][1] });
				gb->AddComponent<Engine::Core::Components::SpriteComponent>(
					c["source"], ctx.GetResourceManager(), SDL_FRect{ c["rect"][0], c["rect"][1],
					c["rect"][2], c["rect"][3] }, Engine::Maths::Align::CENTER_LEFT);

				if (c.count("transparent")) {
					gb->GetComponent<Engine::Core::Components::SpriteComponent>()->SetColorMod(SDL_Color{
						255, 255, 255, static_cast<u8>(255 * c["transparent"].get<float>()) });
				}

				if (c["id"] == "baku") {
					Engine::Render::Effect::EffectManager::Swing(gb.get(), 20.0f, 0.3f);
					Engine::Render::Effect::EffectManager::BreatheColor(gb.get(), { 255, 255, 0, 255 });
				}

				if (c["type"] == "bcg") {
					vec2[c.at("layer").get<i32>()].push_back(std::move(gb));
				}
				else if (c["type"] == "opt") {
					vec1[c.at("layer").get<i32>()].push_back(std::move(gb));
				}
				else {
					spdlog::error("错误类型");
					return;
				}
			}
			opts = std::move(vec1);
			bcgs = std::move(vec2);
			is_init = true;
		}
		catch (const std::exception& err) {
			spdlog::error("初始化暂停UI发生错误:{}", err.what());
			return;
		}
	}

	void PausePanel::HandleInput(Engine::Core::Context& ctx, Engine::Audio::AudioManager& au, 
		Engine::Scene::SceneManager& sm)
	{
		if (!is_init) {
			return;
		}


		auto& input = ctx.GetInputManager();

		if (input.IsActionPressed("move_forward")) {
			--index;
		}
		else if (input.IsActionPressed("move_back")) {
			++index;
		}
		if (index < 0) {
			index = opts[layer].size() - 1;
		}
		else if (index >= opts[layer].size()) {
			index = 0;
		}

		if (old_index != index) {
			if (old_index != -1) {
				au.PlaySFX("se_select00", "select");
				Engine::Render::Effect::EffectManager::StopBreatheColor(opts[layer][old_index].get());
			}
			Engine::Render::Effect::EffectManager::BreatheColor(opts[layer][index].get(), SDL_Color{255, 0, 0, 255});
		}

		if (input.IsActionPressed("esc")) {
			au.PlaySFX("se_cancel00", "select");
			Engine::Render::Effect::EffectManager::StopBreatheColor(opts[layer][index].get());
			--layer;
			if (layer < 1) {
				layer = 1;
				ui.RequestPopPanel();
			}
			index = static_cast<i32>(type);
			Engine::Render::Effect::EffectManager::BreatheColor(opts[layer][index].get(), SDL_Color{ 255, 0, 0, 255 });
		}
		if (input.IsActionPressed("ok")) {
			au.PlaySFX("se_ok00", "select");
			if (layer == 1) {
				OptChooseLayer1(ctx, au, sm);
			}
			else {
				OptChooseLayer2(ctx, au, sm);
			}
			
		}


		old_index = index;
	}

	void PausePanel::Update(f32 d_t, Engine::Core::Context& ctx)
	{
		for (int i = 0; i <= layer; ++i) {
			for (auto& bcg : bcgs[i]) {
				bcg->Update(d_t, ctx);
			}
			for (auto& opt : opts[i]) {
				opt->Update(d_t, ctx);
			}
		}
	}

	void PausePanel::Render(Engine::Core::Context& ctx)
	{
		for (int i = 0; i <= layer; ++i) {
			for (auto& bcg : bcgs[i]) {
				bcg->Render(ctx);
			}
			for (auto& opt : opts[i]) {
				opt->Render(ctx);
			}
		}
	}

	void PausePanel::OnEnter()
	{
		for (int i = 0; i <= layer; ++i) {
			for (auto& bcg : bcgs[i]) {
				if (bcg->GetName() == "black_blur") {
					Engine::Render::Effect::EffectManager::FadeIn(bcg.get(), 0.5f, 0, 204);
				}
				else {
					Engine::Render::Effect::EffectManager::FadeIn(bcg.get(), 0.2f);
					Engine::Render::Effect::EffectManager::SlideIn(bcg.get(), 0.5f, {-100.0f, 0.0f});
				}
			}
			for (auto& opt : opts[i]) {
				Engine::Render::Effect::EffectManager::SlideIn(opt.get(), 0.5f, { -100.0f, 0.0f });
			}
		}
	}

	void PausePanel::OnActivate()
	{ }

	void PausePanel::OnLeave()
	{ }

	void PausePanel::OptChooseLayer1(Engine::Core::Context& ctx, Engine::Audio::AudioManager& au,
		Engine::Scene::SceneManager& sm)
	{
		switch (index) {
		case 0: {
			ui.RequestPopPanel();
			ctx.GetInputManager().ResetAllState();
		}break;
		case 1: {
			Engine::Render::Effect::EffectManager::StopBreatheColor(opts[layer][index].get());
			++layer;
			if (layer > 2) {
				layer = 2;
			}
			index = 1;
			type = OptType::LEAVE;
		}break;
		case 2: {
			Engine::Render::Effect::EffectManager::StopBreatheColor(opts[layer][index].get());
			++layer;
			if (layer > 2) {
				layer = 2;
			}
			index = 1;
			type = OptType::RESTART;
		}
		}
		Engine::Render::Effect::EffectManager::BreatheColor(opts[layer][index].get(), SDL_Color{ 255, 0, 0, 255 });
	}

	void PausePanel::OptChooseLayer2(Engine::Core::Context& ctx, Engine::Audio::AudioManager& au, 
		Engine::Scene::SceneManager& sm)
	{
		switch (index) {
		case 0: {
			if (type == OptType::LEAVE) {
				ResetData();
				sm.RequestReplaceScene(std::make_unique<Game::Scene::TitleScene>("title", ctx, sm, au, ui));
				ui.Clear();
			}
			else if (type == OptType::RESTART) {
				sm.GetCurrentScene()->Reset();
			}
		}break;
		case 1:
		default: {
			--layer;
			if (layer < 1) {
				layer = 1;
			}
			index = static_cast<i32>(type);
			Engine::Render::Effect::EffectManager::BreatheColor(opts[layer][index].get(), SDL_Color{ 255, 0, 0, 255 });
		}break;
		}
	}
}