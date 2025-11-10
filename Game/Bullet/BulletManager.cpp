#include "BulletManager.h"
#include "Bullet.h"
#include "BulletComponent.h"
#include "../../Engine/AudioManager.h"
#include "../../Engine/maths.h"
namespace Game::Manager {
	using namespace Game::Bullets;

	BulletManager::BulletManager(Engine::Core::Context& _ctx,
		Engine::Audio::AudioManager& _audio)
		:ctx(_ctx), audio(_audio), index(0)
	{
		
		BulletData bd;
		bd.sprite_name = "bullet1";
		bd.position = { 0.0f, 0.0f };
		bd.angle = 0.0f;
		bd.angle_acc = 0.0f;
		bd.graze_size = { 15.0f, 15.0f };
		bd.hit_size = { 10.0f, 10.0f };
		bd.rect = { 0.0f, 16.0f, 16.0f, 16.0f };
		bd.speed = 0.0f;
		bd.v = { 0.0f, 0.0f };
		bd.is_grazed = false;
		bd.is_active = false;
		bullets.reserve(MAX_BULLET_SIZE);

		for (size_t i = 0; i < MAX_BULLET_SIZE; ++i) {
			bullets.emplace_back(bd, ctx);
		}
	}

	BulletManager::~BulletManager()
	{

	}


	void BulletManager::SpawnSingleBullet(const Vec2& position, float speed, float angle, 
		float speed_acc, float angle_acc, float sprite_offset)
	{
		if (angle >= 360.0f) {
			angle = fmod(angle, 360.0f);
		}

		if (index == bullets.size() - 1) {
			index = 0;
		}

		auto& data = bullets[index].GetBulletData();
		auto trans = bullets[index].GetGameObject()->GetComponent<Engine::Core::Components::TransformComponent>();
		auto sprite = bullets[index++].GetGameObject()->GetComponent<Engine::Core::Components::SpriteComponent>();
		trans->SetPosition(position);
		data.position = position;
		data.angle = angle;
		//test
		{
			sprite->SetRect({ sprite_offset, 16.0f, 16.0f, 16.0f });
		}
		trans->SetRotationDeg(angle + 90.0f);	//角度问题

		data.speed = speed;
		data.angle_acc = angle_acc;
		data.speed_acc = speed_acc;
		data.is_active = true;
		data.v = { data.speed * cos(Engine::Maths::DegToRad(data.angle)), data.speed * sin(Engine::Maths::DegToRad(data.angle)) };
	}

	void BulletManager::SpawnBulletPattern(Bullets::AimMode mode)
	{

	}

	void BulletManager::RemoveAllBullets(bool turn_into_items)
	{

	}

	void BulletManager::DespawnBullet()
	{

	}


	void BulletManager::Update(float d_t) {
		for (auto& bullet : bullets) {
			auto& data = bullet.GetBulletData();
			if (data.is_active) {
				auto obj = bullet.GetGameObject();
				bullet.Update(d_t);
			}
		}
	}

	void BulletManager::Render() {
		for (auto& bullet : bullets) {
			auto& data = bullet.GetBulletData();
			if (data.is_active) {
				auto obj = bullet.GetGameObject();
				bullet.Render();
			}
		}
	}

}