#include "TitleScene.h"
#include "Stage.h"
#include "../../Engine/context.h"
#include "../../Engine/maths.h"
#include "../../Engine/InputManager.h"
#include "../../Engine/SceneManager.h"
#include "../../Engine/EffectManager.h"
#include "../../Engine/Components/TransformComponent.h"
#include "../../Engine/Components/SpriteComponent.h"
#include "../../Engine/Components/AnimationComponent.h"



namespace Game::Scene {
	TitleScene::TitleScene(const std::string& name, Engine::Core::Context& ctx, Engine::Scene::SceneManager& s_m,
		Engine::Audio::AudioManager& _au)
		:Engine::Scene::Scene(name, ctx, s_m), index(0), old_index(-1), au(_au), first(true)
	{ }

	void TitleScene::Init()
	{
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
				"title_item00", ctx.GetResourceMannager(), coup.second
			);
			
			opts.push_back(std::move(gb));

			coup.second.x -= 256.0f;
			gb = std::make_unique<Engine::Core::GameObject>();
			gb->AddComponent<TransformComponent>(
				coup.first);
			gb->AddComponent<SpriteComponent>(
				"title_item00", ctx.GetResourceMannager(), coup.second
			);
			gb->AddComponent<AnimationComponent>();
			auto animation_component = gb->GetComponent<AnimationComponent>();
			Engine::Render::Animation anim_start("title_blink");
			for (int i = 0; i < 20; ++i) {
				float t = i / 19.0f;
				Uint8 alpha = static_cast<Uint8>(64 + 191 * (0.7f + 0.3f * sin(t * 6.28f)));
				anim_start.AddFrame(
					coup.second, 0.05f,
					{ 1.0f, 1.0f }, 0.0f,
					{ 255, 255, 255, alpha },
					AnimationFrame::Curve::LINEAR
				);
			}
			animation_component->AddAnimation(std::make_unique<Engine::Render::Animation>(anim_start));
			gb->GetComponent<SpriteComponent>()->SetHidden(true);
			opts.push_back(std::move(gb));
		}

		std::vector<std::pair<std::string, std::pair<Engine::Maths::Vec2, SDL_FRect>>> info_bcg{
			{"title_bk00",{{0.0f, 0.0f} , {0.0f, 0.0f, 1280.0f, 960.0f}}},
			{"title_pattern1",{{500.0f, 250.0f},{0.0f, 0.0f, 691.0f, 355.0f}} },
			{"title" ,{{450.0f, 20.0f},{0.0f, 0.0f, 800.0f, 400.0f}}},
			{"title_ver",{{10.0f, 920.0f},{0.0f, 0.0f, 256.0f, 32.0f}} }
		};

		for (auto& coup : info_bcg) {
			gb = std::make_unique<Engine::Core::GameObject>();
			gb->AddComponent<Engine::Core::Components::TransformComponent>(
				coup.second.first);
			gb->AddComponent<Engine::Core::Components::SpriteComponent>(
				coup.first, ctx.GetResourceMannager(), coup.second.second
			);
			bcgs.push_back(std::move(gb));
		}


	}

	void TitleScene::HandleInput()
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
		else if(index < 0) {
			index = static_cast<i16>(opts.size() - 1) / 2;
		}

		if (index != old_index) {
			if (!first) {
				au.PlaySFX("se_select00", "select");
			}
			auto opt = opts[index * 2 + 1].get();
			opt->GetComponent<Engine::Core::Components::AnimationComponent>()->StopAnimaiton();
			opt->GetComponent<Engine::Core::Components::AnimationComponent>()->PlayAnimaiton("title_blink");
			Engine::Render::Effect::EffectManager::Shake(opt, 0.12f, 4.0f);
			first = false;
		}

		if (input.IsActionPressed("ok")) {
			au.PlaySFX("se_ok00", "select");
			switch (index) {
			case 0: {
				std::unique_ptr<Game::Scene::Stage> stage = std::make_unique<Game::Scene::Stage>(
					"", ctx, scene_manager, au);
				scene_manager.RequestReplaceScene(std::move(stage));
			}break;
			case 1: {

			}break;
			case 2: {

			}break;
			case 3: {
				ctx.GetInputManager().SetShouldQuit(true);
			}break;
			}
		}

		old_index = index;
	}

	void TitleScene::Update(float dt)
	{
		for (int i = 0; i < opts.size();++i) {
			auto& opt = opts[i];
			if (i == index * 2 + 1) {
				opt->GetComponent<Engine::Core::Components::SpriteComponent>()->SetHidden(false);
			}
			else if(i % 2 == 1) {
				opt->GetComponent<Engine::Core::Components::SpriteComponent>()->SetHidden(true);
			}

			opt->Update(dt, ctx);
		}
	}

	void TitleScene::Render()
	{
		for (auto& bcg : bcgs) {
			bcg->Render(ctx);
		}
		for (auto& opt : opts) {
			opt->Render(ctx);
		}

	}
}
