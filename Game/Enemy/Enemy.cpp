#include "Enemy.h"
#include "../../Engine/GameObject.h"
#include "../../Engine/Components/TransformComponent.h"
#include "../../Engine/Components/SpriteComponent.h"
#include "../../Engine/Components/ColliderComponent.h"
#include "../../Engine/Components/AnimationComponent.h"
#include "../../Engine/AudioManager.h"
#include "EnemyComponent.h"
#include "../../Engine/collider.h"
namespace Game::Entity {
	Enemy::Enemy(EnemyData&& _ed, Engine::Core::Context& _ctx, Engine::Audio::AudioManager& _au)
		:ctx(_ctx)
	{
		body = std::make_unique<Engine::Core::GameObject>();
		body->AddComponent<Engine::Core::Components::TransformComponent>(
			_ed.position, _ed.scale);
		body->AddComponent<Engine::Core::Components::SpriteComponent>(
			_ed.sprite_name, _ctx.GetResourceManager(), _ed.rect, Engine::Maths::Align::CENTER);
		body->AddComponent<Engine::Core::Components::AnimationComponent>();
		body->AddComponent<Engine::Core::Components::ColliderComponent>(
			std::make_unique<Engine::Core::Collider::AABBCollider>(_ed.hit_box),
			Engine::Maths::Align::CENTER);
		body->AddComponent<Game::Component::EnemyComponent>(std::move(_ed), _au);
	}

	void Enemy::Update(f32 d_t)
	{
		body->Update(d_t, ctx);
	}

	void Enemy::Render()
	{
		body->Render(ctx);
	}

	void Enemy::Despawn()
	{
		auto e = GetEnemyComponent();
		if (!e) {
			return;
		}
		auto& ed = e->GetEnemyData();
		ed.is_active = false;
	}
}