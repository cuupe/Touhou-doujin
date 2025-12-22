#pragma once
#include "../GameData.h"
#include "../../Engine/maths.h"
#include "../../Engine/context.h"
#include "../../Engine/component.h"
#include "../../Engine/Components/TransformComponent.h"
#include "../../Engine/Components/SpriteComponent.h"
#include "../../Engine/Components/ColliderComponent.h"
#include "../../Engine/Components/AnimationComponent.h"
namespace Engine::Audio {
	class AudioManager;
}
namespace Game::Entity {
	struct EnemyConfig;

	struct EnemyData {
		std::string sprite_name;
		SDL_FRect rect;
		Engine::Maths::Vec2 position;
		Engine::Maths::Vec2 scale;
		Engine::Maths::Vec2 hit_box;
		i32 health;
		i32 drop_id;
		bool is_active = false;
		bool is_boss = false;
		bool enable_body_collision = false;
		const EnemyConfig* ecf = nullptr;
	};
}
namespace Game::Component {
	class EnemyComponent final :public Engine::Core::Component {
		friend class Engine::Core::GameObject;
	private:
		TransformComponent* transform = nullptr;
		SpriteComponent* sprite = nullptr;
		ColliderComponent* collider = nullptr;
		AnimationComponent* anim = nullptr;

	private:
		Game::Entity::EnemyData ed;
		Engine::Audio::AudioManager& au;


	public:
		EnemyComponent(Game::Entity::EnemyData&& bd, Engine::Audio::AudioManager& _au);
		EnemyComponent(Game::Entity::EnemyData& bd, Engine::Audio::AudioManager& _au);

	public:
		Engine::Core::GameObject* GetOwner() const { return owner; }
		Game::Entity::EnemyData& GetEnemyData() { return ed; }
		void SetEnemyData(const Game::Entity::EnemyData& _ed) { ed = _ed; }
		void SetDead();
		bool Damage(i32 damage) {
			ed.health -= damage; 
			if (ed.health <= 0.0f) {
				SetDead(); 
				return true;
			}
			return false;
		}
		
	private:
		void Init() override;
		void Update(float, Engine::Core::Context&) override {};
	};
}