#include "BulletComponent.h"

namespace Game::Component {
	BulletComponent::BulletComponent()
	{
		spdlog::trace("Bullet组件初始化成功");
	}



	void BulletComponent::Init()
	{
		if (!owner) {
			spdlog::error("Bullet未挂载游戏对象");
			return;
		}
		transform = owner->GetComponent<TransformComponent>();
		sprite = owner->GetComponent<SpriteComponent>();
		collider = owner->GetComponent<ColliderComponent>();
		if (!transform || !sprite || !collider) {
			spdlog::error("不能获取关键组件，请检查组件加载");
			return;
		}

	}

	void BulletComponent::Update(float d_t, Engine::Core::Context& ctx)
	{
		if (!owner) {
			spdlog::error("Bullet未挂载游戏对象");
			return;
		}


	}


}