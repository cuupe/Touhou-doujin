#pragma once
#include "Bullet.h"
#include "../../Engine/context.h"
#include "../GameData.h"
#include "../Enemy/Enemy.h"
namespace Game::Manager {
	class BulletManager final {
	private:
		Engine::Core::Context& ctx;
		Game::Bullets::Bullet bullets[1024];
		Game::Bullets::Laser lasers[64];
		Game::Bullets::BulletTypeSprites bullet_type_templates[16];
		i32 next_bullet_index;
		i32 bullet_count;

	public:
		BulletManager(Engine::Core::Context&);
		~BulletManager();

	public:
		void SpawnSingleBullet(Game::GameObject::EnemyBulletShooter*, i32, i32, f32);
		void RemoveAllBullets();
		void DespawnBullet();

		void Update(float);
		void Render();
	};
}