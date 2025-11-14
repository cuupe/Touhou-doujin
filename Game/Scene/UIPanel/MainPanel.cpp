#include "MainPanel.h"
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
namespace Game::UI {
	void MainMenuPanel::Init(Engine::Core::Context& ctx){
		using namespace Engine::Core::Components;
		index = 0;
		std::unique_ptr<Engine::Core::GameObject> gb;
		std::vector<std::pair<Engine::Maths::Vec2, SDL_FRect>> info{
			{{100.0f, 600.0f} , {0.0f, 0.0f, 256.0f, 64.0f}},
			{{100.0f, 680.0f} , {0.0f, 64.0f * 6.0f, 256.0f, 64.0f}},
			{{100.0f, 760.0f} , {0.0f, 64.0f * 7.0f, 256.0f, 64.0f}},
			{{100.0f, 840.0f} , {0.0f, 64.0f * 8.0f, 256.0f, 64.0f}}
		};
		
		for (auto& coup : info) {
			gb = std::make_unique<Engine::Core::GameObject>();
			gb->AddComponent<TransformComponent>(
				coup.first);
			coup.second.x += 256.0f;
			gb->AddComponent<SpriteComponent>(
				"title_item00", ctx.GetResourceManager(), coup.second
			);
			opts.push_back(std::move(gb));

			coup.second.x -= 256.0f;
			gb = std::make_unique<Engine::Core::GameObject>();
			gb->AddComponent<TransformComponent>(
				coup.first);
			gb->AddComponent<SpriteComponent>(
				"title_item00", ctx.GetResourceManager(), coup.second
			);
			gb->GetComponent<SpriteComponent>()->SetHidden(true);
			opts.push_back(std::move(gb));
		}

		std::vector<std::pair<std::string, std::pair<Engine::Maths::Vec2, SDL_FRect>>> info_bcg{
			{"title_pattern1",{{500.0f, 250.0f},{0.0f, 0.0f, 691.0f, 355.0f}} },
			{"title" ,{{450.0f, 20.0f},{0.0f, 0.0f, 800.0f, 400.0f}}}
		};

		for (size_t i = 0; i < info_bcg.size(); ++i) {
			auto& coup = info_bcg[i];
			gb = std::make_unique<Engine::Core::GameObject>();
			gb->AddComponent<Engine::Core::Components::TransformComponent>(coup.second.first);
			gb->AddComponent<Engine::Core::Components::SpriteComponent>(coup.first, 
				ctx.GetResourceManager(), coup.second.second);
			bcgs.push_back(std::move(gb));
		}

	}

	void MainMenuPanel::HandleInput(Engine::Core::Context& ctx, Engine::Audio::AudioManager& au, Engine::Scene::SceneManager& sm)
	{
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

		if (input.IsActionPressed("esc")) {
			index = 0;
			au.PlaySFX("se_select00", "select");
			Engine::Render::Effect::EffectManager::StopBlink(opts[old_index * 2 + 1].get());
			Engine::Render::Effect::EffectManager::Shake(opts[index * 2 + 1].get(), 0.1f, 4.0f);
			Engine::Render::Effect::EffectManager::Blink(opts[index * 2 + 1].get());
		}

		if (input.IsActionPressed("ok")) {
			au.PlaySFX("se_ok00", "select");
			Engine::Render::Effect::EffectManager::OKFlash(opts[index * 2 + 1].get());
			Engine::Render::Effect::EffectManager::OKFlash(opts[index * 2].get());
			OptChoose(ctx);
		}


		old_index = index;
	}

	void MainMenuPanel::Update(f32 dt, Engine::Core::Context& ctx)
	{
		for (auto& bcg : bcgs) {
			bcg->Update(dt, ctx);
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

	void MainMenuPanel::Render(Engine::Core::Context& ctx)
	{
		for (auto& bcg : bcgs) {
			bcg->Render(ctx);
		}
		for (auto& opt : opts) {
			opt->Render(ctx);
		}
	}

	void MainMenuPanel::OnEnter()
	{
		Engine::Render::Effect::EffectManager::RotateIn(bcgs[0].get(), 0.7f, 270.0f);
		Engine::Render::Effect::EffectManager::RotateIn(bcgs[1].get(), 0.7f, -270.0f);
		for (auto& opt : opts) {
			Engine::Render::Effect::EffectManager::SlideRightIn(opt.get(), 0.4f, SCREEN_WIDTH);
		}
	}

	void MainMenuPanel::OnActivate()
	{

	}

	void MainMenuPanel::OnLeave()
	{
		for (auto& bcg : bcgs) {
			Engine::Render::Effect::EffectManager::SlideLeftOut(bcg.get(), 0.4f, -SCREEN_WIDTH / 1.5f);
		}
		for (auto& opt : opts) {
			Engine::Render::Effect::EffectManager::SlideLeftOut(opt.get(), 0.4f, SCREEN_WIDTH / 2.0f);
		}
	}

	void MainMenuPanel::OptChoose(Engine::Core::Context& ctx)
	{
		switch (index) {
		case 0: {
			ctx.GetSchedule().NewSequence().Delay(0.6f,
				[&] {
					OnLeave();
				}).Delay(0.3f,
					[&] {
					scene_manager.RequestReplaceScene(std::make_unique<Game::Scene::Stage>("stage",
						ctx, scene_manager, au));
					});
		}break;
		case 1: {
			ctx.GetSchedule().NewSequence().
				Delay(0.6f,
				[&] {	
					OnLeave(); 
				});
		}break;
		case 2: {
			ctx.GetSchedule().NewSequence().Delay(0.6f,
				[&] {
					OnLeave();
				});
		}break;
		case 3: {
			ctx.GetSchedule().NewSequence().Delay(0.6f,
				[&] {
					ctx.GetInputManager().SetShouldQuit(true);
				});
		}break;
		default:break;
		}
	}
}