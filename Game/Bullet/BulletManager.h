#pragma once
#include "../../Engine/context.h"
#include "../../Engine/AudioManager.h"
#include "../GameData.h"
#include "../Enemy/Enemy.h"
#include "../Enemy/EnemyManager.h"
#include "../Player/Player.h"
#include "Bullet.h"
#include <functional>

namespace Game::Manager {
	class BulletManager final {
	private:
		std::map<std::string, Bullets::BulletConfig> bullet_config;
		std::vector<Bullets::BulletData> bullets;

		Engine::Core::Context& ctx;
		Engine::Audio::AudioManager& audio;
		using OnBulletDestroyCallback = std::function<void(const Engine::Maths::Vec2&, int)>;
		OnBulletDestroyCallback on_bullet_destroy_cb;

	private:
		i32 index;

	public:
		BulletManager(Engine::Core::Context&, Engine::Audio::AudioManager&);
		~BulletManager() = default;

	public:
		void Update(f32);
		void Render();

	public:
		void SetOnBulletDestroyCallback(OnBulletDestroyCallback cb) { on_bullet_destroy_cb = cb; }

		Bullets::BulletData& GetAvailableBullet();
		std::vector<Bullets::BulletData>& GetBulletsData() { return bullets; }
		void SpawnSingleBullet(const std::string&, const Engine::Maths::Vec2&,
			float, float, float speed_acc = 0.0f, float angle_acc = 0.0f,
			int sprite_offset = 0);
		void RemoveAllBullets(bool turn_into_items = true, int drop_id = 2);

		void DespawnBullet(Bullets::BulletData&);
		void CheckCollisions(Entity::Player* player);
	};
}