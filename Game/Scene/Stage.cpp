#include "Stage.h"
#include "../Bullet/BulletManager.h"
#include "../../Engine/Components/ColliderComponent.h"
#include "../Player/Player.h"
#include "../../Engine/InputManager.h"
#include "../Enemy/Enemy.h"
#include "../Bullet/Bullet.h"
#include "../../Engine/render.h"

static float ang = 0.0f;
static float an = 0.0f;
static float deltatime = 0.0f;
static float offset = 0.0f;

namespace Game::Scene {
	Stage::Stage(const std::string& name, Engine::Core::Context& ctx, Engine::Scene::SceneManager& s_m,
		Engine::Audio::AudioManager& au)
		:Scene(name, ctx, s_m)
	{ 
		bm = std::make_unique<Manager::BulletManager>(ctx, au);
		player = std::make_unique<Game::GameObject::Player>(ctx);
	}

	void Stage::Init()
	{

	}

	void Stage::HandleInput()
	{
		auto& input = ctx.GetInputManager();

		if (!is_pause)
		{
			if (input.IsActionDown("test_generate_bullet")) {
				bm->SpawnSingleBullet({ Engine::Maths::GetRandomFloat(100.0f, 1000.0f), 150.0f }, Engine::Maths::GetRandomFloat(5.0f, 15.0f),
					Engine::Maths::RadToDeg(Engine::Maths::GetRandomFloat(0.0f, _PI) + _1_2_PI), 0.0f, 0.0f, static_cast<float>(Engine::Maths::GetRandomInt(0, 14) * 16));
			}
			
			if (deltatime > 0.02f) {
				ang += 0.2f;
				
				if (input.IsActionDown("test")) {
					deltatime = 0.0f;
					//for (float i = 0.0f; i < 360.0f; i += 360.0f/8.0f) {
					//	bm->SpawnSingleBullet({ 200.0f, 150.0f }, 15.0f,
					//		i + 0.3f * ang * ang + ang, 0.0f, 0.0f, static_cast<float>(Engine::Maths::GetRandomInt(0, 14) * 16));
					//}
					//for(offset = 0.0f;offset<=900.0f;offset += 30.0f)
					//bm->SpawnSingleBullet({ 200.0f + offset, 150.0f }, 5.0f,
					//	Engine::Maths::RadToDeg(player->AngleToPlayer({ 200.0f + offset, 150.0f })),
					//	15.0f, 0.0f, static_cast<float>(Engine::Maths::GetRandomInt(0, 14) * 16));
					//for (offset = 0.0f; offset <= 900.0f; offset += 30.0f)
					//bm->SpawnSingleBullet({ 200.0f + offset, 150.0f }, 15.0f,
					//		90.0f + ang, 15.0f, -30.0f, static_cast<float>(Engine::Maths::GetRandomInt(0, 14) * 16));
					for (float i = 0.0f; i < 360.0f; i += 360.0f / 6.0f) {
						bm->SpawnSingleBullet({ 500.0f, 150.0f }, 45.0f,
							i + 4.0f * sin(2.0f * ang) + 30.0f + Engine::Maths::RadToDeg(player->AngleToPlayer({ 500.0f, 150.0f }))
							, 0.0f, 0.0f, static_cast<float>(Engine::Maths::GetRandomInt(0, 14) * 16));
					}

				}
			}
		}
		

		if (input.IsActionReleased("test_pause")) {
			is_pause = !is_pause;
		}

		if (!is_pause)
		player->HandleInput();
	}

	void Stage::Update(float d_t)
	{
		if (!is_pause) {
			deltatime += d_t;
			Scene::Update(d_t);
			bm->Update(d_t);
			player->Update(d_t);
			auto coll = player->GetPlayer()->GetComponent<Engine::Core::Components::ColliderComponent>();
			auto& bullets = bm->GetBulletsPoll();
			for (auto& bullet : bullets) {
				auto& bd = bullet.GetBulletData();
				if (bd.is_active) {
					if (CheckCollision(*coll, 
						*(bullet.GetGameObject()->GetComponent<Engine::Core::Components::ColliderComponent>()))) {
						spdlog::info("撞到了，你个fw");
						player->SetToNormal();
						bd.is_active = false;
						bd.is_grazed = false;
					}

				}
			}


		}

	}

	void Stage::Render()
	{
		SDL_SetRenderDrawColor(ctx.GetRenderer().getSDLRenderer(), 0, 255, 0, 255);
		Scene::Render();
		bm->Render();
		player->Render();
	}
}