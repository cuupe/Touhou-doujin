#pragma once
#include "../Engine/GameObject.h"
#include "../Bullet/Bullet.h"
#include "../../Engine/context.h"
namespace Engine::Audio {
	class AudioManager;
}
namespace Engine::Scene {
	class SceneManager;
}
namespace Engine::UISystem {
	class UIManager;
}
namespace Game::Manager {
	class PlayerBulletManager;
	class EnemyManager;
	class BulletManager;
}

namespace Game::Entity {
	class Player {
	protected:
		std::unique_ptr<Engine::Core::GameObject> role;
		Engine::Audio::AudioManager& au;
		Engine::Core::Context& ctx;
		Engine::Scene::SceneManager& sm;
		Engine::UISystem::UIManager& ui;
		Manager::PlayerBulletManager& pbm;
		Manager::BulletManager& bm;
		Manager::EnemyManager& em;

	protected:
		const f32 muteki_time = 5.0f;
		bool is_muteki = false;
		f32 count_time = 0.0f;
		bool is_over = false;
	public:
		Player(Engine::Core::Context&, const std::string&, 
			Engine::Audio::AudioManager&, Engine::Scene::SceneManager&, Engine::UISystem::UIManager&,
			Game::Manager::PlayerBulletManager&, Manager::EnemyManager&, Manager::BulletManager&);
		virtual ~Player() {};

	public:
		f32 AngleToPlayer(const Engine::Maths::Vec2& pos);
		void SetDead();
		virtual void Shoot();
		virtual std::pair<Engine::Maths::Vec2, float> GetGrazeData() const;
	public:
		Engine::Core::GameObject* GetPlayer() const { return role.get(); }
		bool IsMuteki() const { return is_muteki; }
		bool IsOver() const { return is_over; }
		void Reset();
	public:
		virtual void Update(f32 d_t);
		virtual void HandleInput();
		virtual void Render();
	};
}

