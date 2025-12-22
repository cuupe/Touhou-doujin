#pragma once
#include "../GameData.h"
#include "../../Engine/maths.h"
#include "../../Engine/GameObject.h"
#include "../../Engine/sprite.h"
#include "../../Engine/animation.h"
#include "EnemyComponent.h"
#include "../Bullet/Bullet.h"
namespace Engine::Audio {
	class AudioManager;
}
namespace Game::Entity {
	class EnemyData;
}
namespace Game::Entity {
	class Enemy final {
	private:
		std::unique_ptr<Engine::Core::GameObject> body;
		std::vector<std::unique_ptr<Engine::Core::GameObject>> plus;
		Engine::Core::Context& ctx;

	public:
		Enemy(EnemyData&& ed, Engine::Core::Context& _ctx, Engine::Audio::AudioManager& _au);
		~Enemy() = default;

	public:
		void Update(f32 d_t);
		void Render();

	public:
		Engine::Core::GameObject* GetGameObject() const { return body.get(); }
		Game::Component::EnemyComponent* GetEnemyComponent() const { return body->GetComponent<Game::Component::EnemyComponent>(); }
		void Move(const Engine::Maths::Vec2& _dir, float _speed);
	};
}
