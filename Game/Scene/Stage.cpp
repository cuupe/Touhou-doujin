#include "Stage.h"
#include "../Bullet/BulletManager.h"
#include "../../Engine/EffectManager.h"
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
static float d__t;



namespace Game::Scene {
	Stage::Stage(const std::string& name, Engine::Core::Context& ctx, Engine::Scene::SceneManager& s_m,
		Engine::Audio::AudioManager& _au)
		:Scene(name, ctx, s_m), au(_au)
	{ 
		bm = std::make_unique<Manager::BulletManager>(ctx, _au);
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
			
			if (deltatime > 0.1f) {
				ang += 0.2f;
				
				if (input.IsActionDown("test")) {
					deltatime = 0.0f;
					offset += 30.0f;
					if (offset > 500.0f) {
						offset = 0.0f;
					}
					//for (float i = 0.0f; i < 360.0f; i += 360.0f/8.0f) {
					//	bm->SpawnSingleBullet({ 200.0f, 150.0f }, 15.0f,
					//		i + 0.3f * ang * ang + ang, 0.0f, 0.0f, static_cast<float>(Engine::Maths::GetRandomInt(0, 14) * 16));
					//}
					for(offset = 0.0f;offset<=900.0f;offset += 30.0f)
					bm->SpawnSingleBullet({ 200.0f + offset, 150.0f }, 5.0f,
						Engine::Maths::RadToDeg(player->AngleToPlayer({ 200.0f + offset, 150.0f })),
						15.0f, 0.0f, static_cast<float>(Engine::Maths::GetRandomInt(0, 14) * 16));
					//for (offset = 0.0f; offset <= 900.0f; offset += 30.0f)
					//bm->SpawnSingleBullet({ 200.0f + offset, 150.0f }, 15.0f,
					//		0.0f, 0.0f, 0.0f, static_cast<float>(Engine::Maths::GetRandomInt(0, 14) * 16));
					//for (float i = 0.0f; i < 120.0f; i += 120.0f / 6.0f) {
					//	bm->SpawnSingleBullet({ 400.0f, 150.0f }, 60.0f,
					//		i + 2.0f * sin(3.0f * ang) + Engine::Maths::RadToDeg(player->AngleToPlayer({ 400.0f, 150.0f })) - 50.0f
					//		, 0.0f, 0.0f, static_cast<float>(Engine::Maths::GetRandomInt(0, 14) * 16));
						//bm->SpawnSingleBullet({ 900.0f, 150.0f }, 45.0f,
						//	i + 2.0f * sin(3.0f * ang) - 60.0f + Engine::Maths::RadToDeg(player->AngleToPlayer({ 900.0f, 150.0f }))
						//	, 0.0f, 0.0f, static_cast<float>(Engine::Maths::GetRandomInt(0, 14) * 16));
					//}
					au.PlaySFX("se_tan01", "shoot");
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
		d__t = d_t;
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
					if (Engine::Core::Collider::CheckCollision(*coll, 
						*(bullet.GetGameObject()->GetComponent<Engine::Core::Components::ColliderComponent>()))) {
						auto player_core = player->GetPlayer();
						if (!(player_core->HasComponent<Engine::Render::Effect::FlashEffect>())) {
							player->SetToNormal();
							bm->RemoveAllBullets(false);
							au.PlaySFX("se_pldead00", "player");
							Engine::Render::Effect::EffectManager::Flash(player_core, 3.0f);
						}
						else {
							bm->DespawnBullet(bullet);
						}
					}

				}
			}
		}

	}

	void Stage::Render()
	{
		Scene::Render();
		bm->Render();
		player->Render();
	}
}