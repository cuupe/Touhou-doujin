#pragma once
#include "../../Engine/context.h"
#include "../../Engine/AudioManager.h"
#include "../GameData.h"
#include "../Enemy/Enemy.h"
#include "../Player/Player.h"
#include "Bullet.h"
namespace Game::Manager {
	class BulletManager final {
	private:
		std::vector<Bullets::Bullet> bullets;
		Engine::Core::Context& ctx;
		Engine::Audio::AudioManager& audio;

	private:
		i32 index;

	public:
		BulletManager(Engine::Core::Context&, Engine::Audio::AudioManager&);
		~BulletManager();

	public:
		std::vector<Bullets::Bullet>& GetBulletsPoll() { return bullets; }

	public:
		void SpawnSingleBullet(const Vec2&, float, float, float speed_acc = 0.0f, float angle_acc = 0.0f, float sprite_offset = 0.0f);
		void SpawnBulletPattern(Bullets::AimMode);
		void RemoveAllBullets(bool);
		void DespawnBullet();

		void Update(float);
		void Render();
	};
}