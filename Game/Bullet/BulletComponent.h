#pragma once
#include "../../Engine/maths.h"
#include "../../Engine/context.h"
#include "../../Engine/component.h"
#include "../../Engine/Components/TransformComponent.h"
#include "../../Engine/Components/SpriteComponent.h"
#include "../../Engine/Components/ColliderComponent.h"
#include "Bullet.h"
namespace Game::Component {
	class BulletComponent final :public Engine::Core::Component {
		friend class Engine::Core::GameObject;
	private:
		TransformComponent* transform = nullptr;
		SpriteComponent* sprite = nullptr;
		ColliderComponent* collider = nullptr;

	private:

	public:
		BulletComponent();

	public:
		Engine::Core::GameObject* GetOwner() const { return owner; }

	private:
		void Init() override;
		void Update(float, Engine::Core::Context&) override;
	};
}