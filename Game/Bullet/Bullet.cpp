#include "Bullet.h"
#include "../Engine/GameObject.h"
#include "../Engine/Components/TransformComponent.h"
#include "../Engine/Components/SpriteComponent.h"
#include "../Engine/Components/ColliderComponent.h"
#include "../Engine/Components/AnimationComponent.h"
#include "BulletComponent.h"
namespace Game::Bullets {
	Bullet::Bullet(Game::Bullets::BulletData* bullet_data)
		:bd(bullet_data)
	{
		core = std::make_unique<Engine::Core::GameObject>("", "bullet");
		core->AddComponent<Engine::Core::Components::TransformComponent>(
			bd->position, Vec2{2.0f, 2.0f}
		);
		core->AddComponent<Engine::Core::Components::SpriteComponent>(

		);
		core->AddComponent<Engine::Core::Components::ColliderComponent>(

		);
		core->AddComponent<Engine::Core::Components::AnimationComponent>();
		auto anim = core->GetComponent<Engine::Core::Components::AnimationComponent>();
		anim->AddAnimation(std::make_unique<Engine::Render::Animation>(bd->sprites.anim));
	}
}