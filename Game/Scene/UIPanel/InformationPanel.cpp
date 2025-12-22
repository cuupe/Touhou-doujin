#include "InformationPanel.h"
#include "MainPanel.h"
#include "../../../Engine/Components/TransformComponent.h"
#include "../../../Engine/Components/SpriteComponent.h"
#include "../../../Engine/EffectManager.h"
#include "../../../Engine/InputManager.h"
#include "../../../Engine/UIManager.h"
#include "../../../Engine/AudioManager.h"
#include "../../../Engine/schedule.h"
#include "../../../Engine/maths.h"
#include "../../../Engine/text.h"
#include "../../../Engine/context.h"
#include <fstream>
#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>

namespace Game::UI {
	using json = nlohmann::json;

	void InformationPanel::Init(Engine::Core::Context& ctx)
	{
		try {
			std::ifstream f("data/information_panel.json");
			json config;
			f >> config;

			auto& information = config.at("information_panel");
			std::unique_ptr<Engine::Core::GameObject> info_bcg =
				std::make_unique<Engine::Core::GameObject>();
			info_bcg->AddComponent<Engine::Core::Components::TransformComponent>(
				Engine::Maths::Vec2{ information.at("position").at(0).get<f32>(),
				information.at("position").at(1).get<f32>() });

			// [修改 1] 初始化背景颜色 alpha 为 0，实现进场淡入效果
			info_bcg->AddComponent<Engine::Core::Components::SpriteComponent>(
				information.at("source").get<std::string>(), ctx.GetResourceManager(),
				SDL_FRect{ information.at("rect").at(0).get<f32>(),
				information.at("rect").at(1).get<f32>(),
				information.at("rect").at(2).get<f32>(),
				information.at("rect").at(3).get<f32>() }, Engine::Maths::Align::NONE,
				Engine::Maths::Vec2{ 1.0f, 1.0f }, SDL_Color{ 255, 255, 255, 0 }); // 这里改为 0

			info_bcgs.push_back(std::move(info_bcg));

			auto& title_config = information.at("title_config");
			info.title = information.at("title").get<std::string>();
			info.title_config.font_size = title_config.at("font_size").get<f32>();
			info.title_config.color = SDL_Color{
				title_config.at("color").at(0).get<u8>(),
				title_config.at("color").at(1).get<u8>(),
				title_config.at("color").at(2).get<u8>(),
				title_config.at("color").at(3).get<u8>()
			};
			info.title_config.line_spacing = title_config.at("line_spacing").get<f32>();

			auto& content_config = information.at("content_config");
			for (const auto& str : information.at("content")) {
				info.content.emplace_back(str.get<std::string>());
			}
			info.content_config.font_size = content_config.at("font_size").get<f32>();
			info.content_config.color = SDL_Color{
				content_config.at("color").at(0).get<u8>(),
				content_config.at("color").at(1).get<u8>(),
				content_config.at("color").at(2).get<u8>(),
				content_config.at("color").at(3).get<u8>()
			};
			info.content_config.line_spacing = content_config.at("line_spacing").get<f32>();

			std::unique_ptr<Engine::Core::GameObject> gb = std::make_unique<Engine::Core::GameObject>();
			gb->AddComponent<TransformComponent>(
				std::move(Engine::Maths::Vec2{ information["base_position"][0], information["base_position"][1] }));
			gb->AddComponent<SpriteComponent>(
				information["normal_sprite"]["source"], ctx.GetResourceManager(), std::move(SDL_FRect{
					information["normal_sprite"]["rect"][0],information["normal_sprite"]["rect"][1],
					information["normal_sprite"]["rect"][2],information["normal_sprite"]["rect"][3] }));
			opts.push_back(std::move(gb));

			gb = std::make_unique<Engine::Core::GameObject>();
			gb->AddComponent<TransformComponent>(
				std::move(Engine::Maths::Vec2{ information["base_position"][0], information["base_position"][1] }));
			gb->AddComponent<SpriteComponent>(
				information["selected_sprite"]["source"], ctx.GetResourceManager(), std::move(SDL_FRect{
					information["selected_sprite"]["rect"][0],information["selected_sprite"]["rect"][1],
					information["selected_sprite"]["rect"][2],information["selected_sprite"]["rect"][3] }));
			Engine::Render::Effect::EffectManager::Blink(gb.get(), 3.0f);
			opts.push_back(std::move(gb));
		}
		catch (const std::exception& err) {
			spdlog::error("加载配置文件发生错误{}", err.what());
			return;
		}

		// [修改 2] 初始 alpha 设为 0.0f，配合 Update 逻辑实现淡入
		alpha = 0.0f;
		is_init = true;
	}

	void InformationPanel::HandleInput(Engine::Core::Context& ctx, Engine::Audio::AudioManager& au, Engine::Scene::SceneManager& sm)
	{
		if (!is_init) {
			return;
		}

		auto& input = ctx.GetInputManager();

		if (input.IsActionPressed("esc") || input.IsActionPressed("ok")) {
			is_init = false;
			au.PlaySFX("se_ok00", "select");

			if (opts.size() > 0) Engine::Render::Effect::EffectManager::OKFlash(opts[0].get());
			if (opts.size() > 1) Engine::Render::Effect::EffectManager::OKFlash(opts[1].get());
			ctx.GetSchedule().NewSequence().Delay(0.6f, [&] {
				OnLeave();
				}).Delay(0.3f, [&] {
					ui.RequestPopPanel();
					});
		}
	}

	void InformationPanel::Update(f32 d_t, Engine::Core::Context& ctx)
	{
		for (const auto& opt : opts) {
			opt->Update(d_t, ctx);
		}
		PanelMainContentUpdate(d_t, ctx);
	}

	void InformationPanel::Render(Engine::Core::Context& ctx)
	{
		for (const auto& b : info_bcgs) {
			b->Render(ctx);
		}
		for (const auto& opt : opts) {
			opt->Render(ctx);
		}
		auto& text = ui.GetTextManager();
		SDL_Color color = info.title_config.color;
		color.a = static_cast<u8>(alpha * 255);
		text.RenderText(info.title, Engine::Maths::Vec2{ 200.0f, 200.0f },
			color, "hwzs", info.title_config.font_size,
			info.title_config.line_spacing, Engine::Maths::Align::CENTER_LEFT);

		color = info.content_config.color;
		color.a = static_cast<u8>(alpha * 255);
		for (int i = 0; i < info.content.size(); ++i) {
			text.RenderText(info.content.at(i), Engine::Maths::Vec2{ 200.0f, 300.0f + i * 30.0f },
				color, "msyh", info.content_config.font_size,
				info.content_config.line_spacing, Engine::Maths::Align::CENTER_LEFT);
		}
	}

	void InformationPanel::PanelMainContentUpdate(f32 d_t, Engine::Core::Context& ctx)
	{
		if (is_init) {
			if (alpha < 1.0f) {
				alpha += d_t * 2.0f;
				if (alpha > 1.0f) alpha = 1.0f;
			}
		}
		else {
			if (alpha > 0.0f) {
				alpha -= d_t * 3.0f;
				if (alpha < 0.0f) alpha = 0.0f;
			}
		}
		for (const auto& info_bcg : info_bcgs) {
			auto* sprite = info_bcg->GetComponent<Engine::Core::Components::SpriteComponent>();
			if (sprite) {
				sprite->SetColorMod({ 255, 255, 255, static_cast<u8>(alpha * 204) });
			}
		}
	}

	void InformationPanel::OnEnter()
	{
		ui.GetAudioManager().PlayBGM("info");
	}

	void InformationPanel::OnActivate()
	{

	}

	void InformationPanel::OnLeave()
	{
		for (const auto& opt : opts) {
			Engine::Render::Effect::EffectManager::SlideLeftOut(opt.get(), 0.5f);
		}
	}
}