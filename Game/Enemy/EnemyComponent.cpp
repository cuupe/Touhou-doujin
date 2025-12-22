#include "EnemyComponent.h"
#include "../../Engine/AudioManager.h"
namespace Game::Component {
	EnemyComponent::EnemyComponent(Game::Entity::EnemyData&& _ed, Engine::Audio::AudioManager& _au)
		:ed(_ed), au(_au)
	{ }

	EnemyComponent::EnemyComponent(Game::Entity::EnemyData& _ed, Engine::Audio::AudioManager& _au)
		:ed(_ed), au(_au)
	{ }

	void EnemyComponent::SetDead()
	{
		if (ed.is_boss) {
			au.PlaySFX("se_enep01", "dead");
		}
		else {
			au.PlaySFX("se_enep00", "dead");
		}
		
		ed.is_active = false;
	}

	void EnemyComponent::Init()
	{
		if (!owner) {
			spdlog::error("Bullet未挂载游戏对象");
			return;
		}
		transform = owner->GetComponent<TransformComponent>();
		sprite = owner->GetComponent<SpriteComponent>();
		collider = owner->GetComponent<ColliderComponent>();
		anim = owner->GetComponent<AnimationComponent>();
		if (!transform || !sprite || !collider) {
			spdlog::error("不能获取关键组件，请检查组件加载");
			return;
		}
	}
}