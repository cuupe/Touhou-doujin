#include "MainPanel.h"
#include "OptionPanel.h"
#include "PlayerDataPanel.h"
#include "InformationPanel.h"
#include "../../../Engine/context.h"
#include "../../../Engine/schedule.h"
#include "../../../Engine/maths.h"
#include "../../../Engine/InputManager.h"
#include "../../../Engine/SceneManager.h"
#include "../Stage.h"
#include "../../../Engine/UIManager.h"
#include "../../../Engine/EffectManager.h"
#include "../../../Engine/Components/TransformComponent.h"
#include "../../../Engine/Components/SpriteComponent.h"
#include "../../../Engine/Components/AnimationComponent.h"
#include <nlohmann/json.hpp>

namespace Game::UI {
	void MainPanel::Init(Engine::Core::Context& ctx){
		using namespace Engine::Core::Components;
		index = 0;
		std::ifstream ascii_file("data/main_panel.json");
		nlohmann::json json;
		ascii_file >> json;
		std::vector<std::unique_ptr<Engine::Core::GameObject>> opts_1;
		opts_1.reserve(20);

		std::unique_ptr<Engine::Core::GameObject> gb;

		for (const auto& c : json["main_panel"]) {
			gb = std::make_unique<Engine::Core::GameObject>();
			gb->AddComponent<TransformComponent>(
				std::move(Engine::Maths::Vec2{c["base_position"][0], c["base_position"][1]}));
			gb->AddComponent<SpriteComponent>(
				c["normal_sprite"]["source"], ctx.GetResourceManager(), std::move(SDL_FRect{
					c["normal_sprite"]["rect"][0],c["normal_sprite"]["rect"][1],
					c["normal_sprite"]["rect"][2],c["normal_sprite"]["rect"][3] }));
			opts_1.push_back(std::move(gb));

			gb = std::make_unique<Engine::Core::GameObject>();
			gb->AddComponent<TransformComponent>(
				std::move(Engine::Maths::Vec2{ c["base_position"][0], c["base_position"][1] }));
			gb->AddComponent<SpriteComponent>(
				c["selected_sprite"]["source"], ctx.GetResourceManager(), std::move(SDL_FRect{
					c["selected_sprite"]["rect"][0],c["selected_sprite"]["rect"][1],
					c["selected_sprite"]["rect"][2],c["selected_sprite"]["rect"][3] }));
			gb->GetComponent<SpriteComponent>()->SetHidden(true);
			opts_1.push_back(std::move(gb));
		}
		opts.push_back(std::move(opts_1));

		for (const auto& c : json["background_title"]) {
			gb = std::make_unique<Engine::Core::GameObject>();
			gb->AddComponent<Engine::Core::Components::TransformComponent>(std::move(Engine::Maths::Vec2{
				c["position"][0], c["position"][1] }));
			gb->AddComponent<Engine::Core::Components::SpriteComponent>(c["source"],
				ctx.GetResourceManager(), std::move(SDL_FRect{
					c["rect"][0], c["rect"][1], c["rect"][2], c["rect"][3]
					}));
			bcgs.push_back(std::move(gb));
		}


		std::vector<std::unique_ptr<Engine::Core::GameObject>> opts_2;
		opts_2.reserve(5);
		for (const auto& c : json["choose"]) {
			gb = std::make_unique<Engine::Core::GameObject>();
			gb->AddComponent<Engine::Core::Components::TransformComponent>(std::move(Engine::Maths::Vec2{
				c["position"][0], c["position"][1] }));
			gb->AddComponent<Engine::Core::Components::SpriteComponent>(c["source"],
				ctx.GetResourceManager(), std::move(SDL_FRect{
					c["rect"][0], c["rect"][1], c["rect"][2], c["rect"][3]
					}), Engine::Maths::Align::CENTER);
			opts_2.push_back(std::move(gb));
		}
		opts.push_back(std::move(opts_2));


		std::vector<std::unique_ptr<Engine::Core::GameObject>> opts_3;
		opts_3.reserve(5);
		for (const auto& c : json["diff"]) {
			gb = std::make_unique<Engine::Core::GameObject>();
			gb->AddComponent<Engine::Core::Components::TransformComponent>(std::move(Engine::Maths::Vec2{
				c["position"][0], c["position"][1] }));
			gb->AddComponent<Engine::Core::Components::SpriteComponent>(c["source"],
				ctx.GetResourceManager(), std::move(SDL_FRect{
					c["rect"][0], c["rect"][1], c["rect"][2], c["rect"][3]
					}), Engine::Maths::Align::CENTER, std::move(Engine::Maths::Vec2{
						c["scale"][0], c["scale"][1]}));
			opts_3.push_back(std::move(gb));
		}
		opts.push_back(std::move(opts_3));

		is_init = true;
	}

	void MainPanel::HandleInput(Engine::Core::Context& ctx, Engine::Audio::AudioManager& au, Engine::Scene::SceneManager& sm)
	{
		if (!is_init) {
			return;
		}

		if (current_state == State::MENU) {
			MenuHandleInput(ctx, au, sm);
		}
		else if (current_state == State::PLAYER) {
			PlayerHandleInput(ctx, au, sm);
		}
		else {
			DiffHandleInput(ctx, au, sm);
		}
	}

	void MainPanel::Update(f32 dt, Engine::Core::Context& ctx)
	{
		if (current_state == State::MENU) {
			MenuUpdate(dt, ctx);
		}
		else if (current_state == State::PLAYER) {
			PlayerUpdate(dt, ctx);
		}
		else {
			DiffUpdate(dt, ctx);
		}
	}

	void MainPanel::Render(Engine::Core::Context& ctx)
	{
		if (current_state == State::MENU) {
			for (auto& bcg : bcgs) {
				bcg->Render(ctx);
			}
		}

		for (auto& opt : opts[static_cast<i16>(current_state)]) {
			opt->Render(ctx);
		}


	}

	void MainPanel::OnEnter()
	{
		if (current_state == State::MENU) {
			MenuOnEnter();
		}
		else if (current_state == State::PLAYER) {
			PlayerOnEnter();
		}
		else {
			DiffOnEnter();
		}
	}

	void MainPanel::OnActivate()
	{
		is_init = true;
	}

	void MainPanel::OnLeave()
	{
		if (current_state == State::MENU) {
			MenuOnLeave();
		}
		else if (current_state == State::PLAYER) {
			PlayerOnLeave();
		}
		else {
			DiffOnLeave();
		}
	}

	void MainPanel::OptChoose(Engine::Core::Context& ctx, Engine::Audio::AudioManager& au,
		Engine::Scene::SceneManager& sm)
	{
		if (current_state == State::MENU) {
			MenuChoose(ctx, au, sm);
		}
		else if (current_state == State::PLAYER) {
			PlayerChoose(ctx, au, sm);
		}
		else {
			DiffChoose(ctx, au, sm);
		}
	}

	void MainPanel::MenuHandleInput(Engine::Core::Context& ctx, Engine::Audio::AudioManager& au, 
		Engine::Scene::SceneManager& sm)
	{
		auto& input = ctx.GetInputManager();

		if (input.IsActionPressed("move_back")) {
			++index;
		}
		else if (input.IsActionPressed("move_forward")) {
			--index;
		}
		if (index > static_cast<i16>(opts[static_cast<i16>(current_state)].size() - 1) / 2) {
			index = 0;
		}
		else if (index < 0) {
			index = static_cast<i16>(opts[static_cast<i16>(current_state)].size() - 1) / 2;
		}
		if (index != old_index) {
			auto opt = opts[static_cast<i16>(current_state)][index * 2 + 1].get();
			if (!first) {
				au.PlaySFX("se_select00", "select");
				Engine::Render::Effect::EffectManager::Shake(opt, 0.1f, 4.0f);
			}
			if (old_index != -1) {
				Engine::Render::Effect::EffectManager::StopBlink(
					opts[static_cast<i16>(current_state)][old_index * 2 + 1].get());
			}
			Engine::Render::Effect::EffectManager::Blink(opt, 2.0f);
			first = false;
		}

		if (input.IsActionPressed("esc")) {
			index = 0;
			au.PlaySFX("se_select00", "select");
			Engine::Render::Effect::EffectManager::StopBlink(
				opts[static_cast<i16>(current_state)][old_index * 2 + 1].get());
			Engine::Render::Effect::EffectManager::Shake(
				opts[static_cast<i16>(current_state)][index * 2 + 1].get(), 0.1f, 4.0f);
			Engine::Render::Effect::EffectManager::Blink(
				opts[static_cast<i16>(current_state)][index * 2 + 1].get());
		}

		if (input.IsActionPressed("ok")) {
			is_init = false;
			au.PlaySFX("se_ok00", "select");
			Engine::Render::Effect::EffectManager::OKFlash(
				opts[static_cast<i16>(current_state)][index * 2 + 1].get());
			Engine::Render::Effect::EffectManager::OKFlash(
				opts[static_cast<i16>(current_state)][index * 2].get());
			OptChoose(ctx, au, sm);
		}


		old_index = index;
	}

	void MainPanel::MenuUpdate(f32 dt, Engine::Core::Context& ctx)
	{
		for (auto& bcg : bcgs) {
			bcg->Update(dt, ctx);
		}
		for (int i = 0; i < opts[static_cast<i16>(current_state)].size(); ++i) {
			auto& opt = opts[static_cast<i16>(current_state)].at(i);
			if (i == index * 2 + 1) {
				opt->GetComponent<Engine::Core::Components::SpriteComponent>()->SetHidden(false);
			}
			else if (i % 2 == 1) {
				opt->GetComponent<Engine::Core::Components::SpriteComponent>()->SetHidden(true);
			}

			opt->Update(dt, ctx);
		}
	}

	void MainPanel::MenuChoose(Engine::Core::Context& ctx, Engine::Audio::AudioManager& au,
		Engine::Scene::SceneManager& sm)
	{
		switch (index) {
		case 0: {
			ctx.GetSchedule().NewSequence().Delay(0.6f,
				[&] {
					OnLeave();
				}).Delay(0.3f,
					[&] {
						current_state = State::PLAYER;
						OnEnter();
					});
		}break;
		case 1: {
			ctx.GetSchedule().NewSequence().
				Delay(0.6f,
					[&] {
						OnLeave();
					}).Delay(0.3f, [&] {
						ui.RequestPushPanel(std::make_unique<OptionPanel>("option", ui));
						});
		}break;
		case 2: {
			ctx.GetSchedule().NewSequence().
				Delay(0.6f,
					[&] {
						OnLeave();
					}).Delay(0.3f, [&] {
						ui.RequestPushPanel(std::make_unique<PlayerDataPanel>("player_data", ui));
						});
		}break;
		case 3: {
			ctx.GetSchedule().NewSequence().Delay(0.6f,
				[&] {
					OnLeave();
				}).Delay(0.3f, [&] {
					ui.RequestPushPanel(std::make_unique<InformationPanel>("info", ui));
					});
		}break;
		case 4: {
			ctx.GetSchedule().NewSequence().Delay(0.6f,
				[&] {
					ctx.GetInputManager().SetShouldQuit(true);
				});
		}break;
		default:break;
		}
	}

	void MainPanel::MenuOnEnter()
	{
		ui.GetAudioManager().PlayBGM("menu");
		Engine::Render::Effect::EffectManager::RotateIn(bcgs[0].get(), 0.7f, 270.0f);
		Engine::Render::Effect::EffectManager::RotateIn(bcgs[1].get(), 0.7f, -270.0f);
		SlideRight();
		is_init = true;
	}

	void MainPanel::MenuOnLeave()
	{
		for (auto& bcg : bcgs) {
			Engine::Render::Effect::EffectManager::SlideLeftOut(bcg.get(), 0.5f, -SCREEN_WIDTH);
		}
		SlideLeft();
	}

	void MainPanel::PlayerHandleInput(Engine::Core::Context& ctx, Engine::Audio::AudioManager& au, 
		Engine::Scene::SceneManager& sm)
	{
		auto& input = ctx.GetInputManager();

		if (input.IsActionPressed("esc")) {
			au.PlaySFX("se_cancel00", "select");
			OnLeave();
			ctx.GetSchedule().NewSequence().Delay(0.3f,
				[&] {
					current_state = State::MENU;
					OnEnter();
				});
		}

		if (input.IsActionPressed("move_left")) {
			au.PlaySFX("se_invalid", "select");
		}
		else if (input.IsActionPressed("move_right")) {
			au.PlaySFX("se_invalid", "select");
		}


		if (input.IsActionPressed("ok")) {
			is_init = false;
			au.PlaySFX("se_ok00", "select");
			Engine::Render::Effect::EffectManager::OKFlash(opts[static_cast<i16>(current_state)][index_player].get());
			PlayerChoose(ctx, au, sm);
		}

	}

	void MainPanel::PlayerChoose(Engine::Core::Context& ctx, Engine::Audio::AudioManager& au,
		Engine::Scene::SceneManager& sm)
	{
		switch (index_player) {
		case 0: {
			ctx.GetSchedule().NewSequence().Delay(0.6f,
				[&] {
					gamedata.id = index_player;
					OnLeave();
				}).Delay(0.3f,
					[&] {
						current_state = State::DIFF;
						OnEnter();
					});
		}break;
		default:break;
		}
	}

	void MainPanel::PlayerUpdate(f32 dt, Engine::Core::Context& ctx)
	{
		for (int i = 0; i < opts[static_cast<i16>(current_state)].size(); ++i) {
			auto& opt = opts[static_cast<i16>(current_state)].at(i);
			opt->Update(dt, ctx);
		}
	}

	void MainPanel::PlayerOnEnter()
	{
		SlideRight();
		is_init = true;
	}

	void MainPanel::PlayerOnLeave()
	{
		SlideLeft();
	}

	void MainPanel::DiffHandleInput(Engine::Core::Context& ctx, Engine::Audio::AudioManager& au, 
		Engine::Scene::SceneManager& sm)
	{
		auto& input = ctx.GetInputManager();

		if (input.IsActionPressed("move_back")) {
			au.PlaySFX("se_select00", "select");
			++index_diff;
		}
		else if (input.IsActionPressed("move_forward")) {
			au.PlaySFX("se_select00", "select");
			--index_diff;
		}
		if (index_diff > static_cast<i16>(opts[static_cast<i16>(current_state)].size() - 1) ) {
			index_diff = 0;
		}
		else if (index_diff < 0) {
			index_diff = static_cast<i16>(opts[static_cast<i16>(current_state)].size() - 1);
		}

		if (input.IsActionPressed("esc")) {
			au.PlaySFX("se_cancel00", "select");
			OnLeave();
			ctx.GetSchedule().NewSequence().Delay(0.3f,
				[&] {
					current_state = State::PLAYER;
					OnEnter();
				});
		}

		if (input.IsActionPressed("ok")) {
			is_init = false;
			au.PlaySFX("se_ok00", "select");
			Engine::Render::Effect::EffectManager::OKFlash(opts[static_cast<i16>(current_state)][index_diff].get());
			DiffChoose(ctx, au, sm);
		}
	}

	void MainPanel::DiffChoose(Engine::Core::Context& ctx, Engine::Audio::AudioManager& au,
		Engine::Scene::SceneManager& sm)
	{
		gamedata.difficulty = index_diff;
		switch (index_diff) {
		case 0:
			gamedata.life = 7 * 5;
			break;
		case 1:
			gamedata.life = 5 * 5;
			break;
		case 2:
			gamedata.life = 3 * 5;
			break;
		case 3:
			gamedata.life = 1 * 5;
			break;
		}
		ctx.GetSchedule().NewSequence().Delay(0.6f,
			[&] {
				OnLeave();
			}).Delay(0.3f,
				[&] {
					sm.RequestReplaceScene(std::make_unique<Game::Scene::Stage>("stage",
						ctx, sm, au, ui));
					ui.RequestPopPanel();
					OnLeave();
				});
	}

	void MainPanel::DiffUpdate(f32 dt, Engine::Core::Context& ctx)
	{
		if (old_diff != -1) {
			opts[static_cast<i16>(current_state)].at(old_diff)->
				GetComponent<SpriteComponent>()->SetColorMod(std::move(SDL_Color{ 255, 255, 255, 255 }));
		}
		for (int i = 0; i < opts[static_cast<i16>(current_state)].size(); ++i) {
			auto& opt = opts[static_cast<i16>(current_state)].at(i);
			if (i == index_diff) {
				opt->GetComponent<SpriteComponent>()->SetColorMod(std::move(SDL_Color{ 255, 0, 0, 255 }));
			}
			opt->Update(dt, ctx);
		}
		old_diff = index_diff;
	}

	void MainPanel::DiffOnEnter()
	{
		SlideRight();
		is_init = true;
	}

	void MainPanel::DiffOnLeave()
	{
		SlideLeft();
	}

	void MainPanel::SlideLeft()
	{
		for (auto& o : opts[static_cast<i16>(current_state)]) {
			Engine::Render::Effect::EffectManager::SlideLeftOut(o.get(),
				0.5, SCREEN_WIDTH_F32);
		}
	}

	void MainPanel::SlideRight()
	{
		for (auto& o : opts[static_cast<i16>(current_state)]) {
			Engine::Render::Effect::EffectManager::SlideRightIn(o.get(),
				0.5, SCREEN_WIDTH_F32);
		}
	}
}