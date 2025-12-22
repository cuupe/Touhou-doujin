#include "OptionPanel.h"
#include "../../../Engine/context.h"
#include "../../../Engine/schedule.h"
#include "../../../Engine/maths.h"
#include "../../../Engine/InputManager.h"
#include "../../../Engine/SceneManager.h"
#include "../../../Engine/UIManager.h"
#include "../../../Engine/text.h"
#include "../../../Engine/EffectManager.h"
#include "../../../Engine/Components/TransformComponent.h"
#include "../../../Engine/Components/SpriteComponent.h"
#include "../../../Engine/Components/AnimationComponent.h"
namespace Game::UI {
	void OptionPanel::Init(Engine::Core::Context& ctx)
	{
		using namespace Engine::Core::Components;

		index = 0;

		std::ifstream ascii_file("data/option_panel.json");
		if (!ascii_file.is_open())
		{
			return;
		}

		nlohmann::json json;
		ascii_file >> json;
		ascii_file.close();

		std::vector<std::unique_ptr<Engine::Core::GameObject>> opts_1;
		opts_1.reserve(20);
		std::unique_ptr<Engine::Core::GameObject> gb;
		for (const auto& c : json["option_panel"])
		{
			if (c.count("name")) {
				gb = std::make_unique<Engine::Core::GameObject>(c.at("name").get<std::string>());
			}
			else {
				gb = std::make_unique<Engine::Core::GameObject>();
			}
			gb->AddComponent<TransformComponent>(
				Engine::Maths::Vec2{ c["base_position"][0], c["base_position"][1] });
			gb->AddComponent<SpriteComponent>(
				c["normal_sprite"]["source"], ctx.GetResourceManager(),
				SDL_FRect{
					c["normal_sprite"]["rect"][0],
					c["normal_sprite"]["rect"][1],
					c["normal_sprite"]["rect"][2],
					c["normal_sprite"]["rect"][3]
				});
			opts_1.push_back(std::move(gb));
			gb = std::make_unique<Engine::Core::GameObject>();
			gb->AddComponent<TransformComponent>(
				Engine::Maths::Vec2{ c["base_position"][0], c["base_position"][1] });
			gb->AddComponent<SpriteComponent>(
				c["selected_sprite"]["source"], ctx.GetResourceManager(),
				SDL_FRect{
					c["selected_sprite"]["rect"][0],
					c["selected_sprite"]["rect"][1],
					c["selected_sprite"]["rect"][2],
					c["selected_sprite"]["rect"][3]
				});
			gb->GetComponent<SpriteComponent>()->SetHidden(true);
			opts_1.push_back(std::move(gb));
		}

		opts = std::move(opts_1);
		is_init = true;
	}

	void OptionPanel::HandleInput(Engine::Core::Context& ctx, Engine::Audio::AudioManager& au, Engine::Scene::SceneManager& sm)
	{
		if (!is_init) {
			return;
		}
		auto& input = ctx.GetInputManager();

		if (input.IsActionPressed("move_back")) {
			++index;
		}
		else if (input.IsActionPressed("move_forward")) {
			--index;
		}
		if (index > static_cast<i16>(opts.size() - 1) / 2) {
			index = 0;
		}
		else if (index < 0) {
			index = static_cast<i16>(opts.size() - 1) / 2;
		}
		if (index != old_index) {
			auto opt = opts[index * 2 + 1].get();
			if (!first) {
				au.PlaySFX("se_select00", "select");
				Engine::Render::Effect::EffectManager::Shake(opt, 0.1f, 4.0f);
			}
			if (old_index != -1) {
				Engine::Render::Effect::EffectManager::StopBlink(opts[old_index * 2 + 1].get());
			}
			Engine::Render::Effect::EffectManager::Blink(opt, 2.0f);
			first = false;
		}
		if (input.IsActionPressed("move_right")) {
			if (index == 1) {
				config.bgm_sound += 5;
				if (config.bgm_sound > 100) {
					config.bgm_sound = 100;
					au.PlaySFX("se_invalid", "select");
				}
				else {
					au.PlaySFX("se_select00", "select");
				}
				sound_changed = true;
			}
			else if (index == 2) {
				config.sfx_sound += 5;
				if (config.sfx_sound > 100) {
					config.sfx_sound = 100;
					au.PlaySFX("se_invalid", "select");
				}
				else {
					au.PlaySFX("se_select00", "select");
				}
				sound_changed = true;
			}
		}
		if (input.IsActionPressed("move_left")) {
			if (index == 1) {
				config.bgm_sound -= 5;
				if (config.bgm_sound < 0) {
					config.bgm_sound = 0;
					au.PlaySFX("se_invalid", "select");
				}
				else {
					au.PlaySFX("se_select00", "select");
				}
				sound_changed = true;
			}
			else if (index == 2) {
				config.sfx_sound -= 5;
				if (config.sfx_sound < 0) {
					config.sfx_sound = 0;
					au.PlaySFX("se_invalid", "select");
				}
				else {
					au.PlaySFX("se_select00", "select");
				}
				sound_changed = true;
			}
		}

		if (input.IsActionPressed("esc")) {
			index = 0;
			au.PlaySFX("se_cancel00", "select");
			ui.RequestPopPanel();
		}

		if (input.IsActionPressed("ok")) {
			au.PlaySFX("se_ok00", "select");
			Engine::Render::Effect::EffectManager::OKFlash(opts[index * 2 + 1].get());
			Engine::Render::Effect::EffectManager::OKFlash(opts[index * 2].get());
			OptChoose(ctx);
		}


		old_index = index;
	}

	void OptionPanel::Update(f32 dt, Engine::Core::Context& ctx)
	{
		if (sound_changed) {
			ui.GetAudioManager().SetBGMSound(config.bgm_sound / 100.0f);
			ui.GetAudioManager().SetSFXSound(config.sfx_sound / 100.0f);
			sound_changed = false;
		}
		for (int i = 0; i < opts.size(); ++i) {
			auto& opt = opts[i];
			if (i == index * 2 + 1) {
				opt->GetComponent<Engine::Core::Components::SpriteComponent>()->SetHidden(false);
			}
			else if (i % 2 == 1) {
				opt->GetComponent<Engine::Core::Components::SpriteComponent>()->SetHidden(true);
			}

			opt->Update(dt, ctx);
		}
	}

	void OptionPanel::Render(Engine::Core::Context& ctx)
	{
		auto& im = ui.GetTextManager();
		for (auto& opt : opts) {
			opt->Render(ctx);
			if (opt->GetName() == "bgm") {
				im.RenderTextASCII(std::to_string(config.bgm_sound) + "%",
					{ opt->GetComponent<TransformComponent>()->GetPosition().x + 500.0f,
					opt->GetComponent<TransformComponent>()->GetPosition().y + 32.0f },
					{ 1.5f, 1.5f }, -5.0f);
			}
			else if (opt->GetName() == "sfx") {
				im.RenderTextASCII(std::to_string(config.sfx_sound) + "%",
					{ opt->GetComponent<TransformComponent>()->GetPosition().x + 500.0f,
					opt->GetComponent<TransformComponent>()->GetPosition().y + 32.0f },
					{ 1.5f, 1.5f }, -5.0f);
			}
		}

	}

	void OptionPanel::OnEnter()
	{
		ui.GetAudioManager().PlayBGM("options");
		for (auto& opt : opts) {
			Engine::Render::Effect::EffectManager::SlideRightIn(opt.get(), 0.4f, -SCREEN_WIDTH);
		}
		OnActivate();
	}

	void OptionPanel::OnActivate()
	{
		is_init = true;
	}

	void OptionPanel::OnLeave()
	{
		for (auto& opt : opts) {
			Engine::Render::Effect::EffectManager::SlideLeftOut(opt.get(), 0.4f, SCREEN_WIDTH / 2.0f);
		}
		is_init = false;
	}

	void OptionPanel::OptChoose(Engine::Core::Context& ctx)
	{
		switch (index) {
		case 0: {
			config = default_config;
			ui.GetAudioManager().SetBGMSound(config.bgm_sound / 100.0f);
			ui.GetAudioManager().SetSFXSound(config.sfx_sound / 100.0f);
		}break;
		case 3: {
			is_init = false;
			ctx.GetSchedule().NewSequence().
				Delay(0.6f,
					[&] {
						OnLeave();
					}).Delay(0.3f, [&] {
						ui.RequestPopPanel();
						});
		}break;
		default:break;
		}
	}


}