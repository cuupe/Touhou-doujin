#pragma once
#include "../GameObject.h"
#include "../Component.h"
#include "States/PlayerState.h"
#include "../maths.h"
#include <memory>
namespace Engine::Core::Components {
	class TransformComponent;
	class SpriteComponent;
	class ColliderComponent;
	class AnimationComponent;
}
namespace Engine::Core::Components::State {
	class PlayerState;
}

namespace Engine::Core::Components {
	class PlayerComponent final :public Component {
		friend class GameObject;

	private:
		TransformComponent* trans = nullptr;
		SpriteComponent* sprite = nullptr;
		ColliderComponent* collider = nullptr;
		AnimationComponent* animation = nullptr;

		std::unique_ptr<State::PlayerState> current_state;

		bool is_slow = false;
		Engine::Maths::Vec2 direct = {0.0f, 0.0f};
		float speed = 100.0f;

	public:
		PlayerComponent(float sp = 100.0f);
		~PlayerComponent() override = default;
		PlayerComponent(const PlayerComponent&) = delete;
		PlayerComponent& operator=(const PlayerComponent&) = delete;
		PlayerComponent(PlayerComponent&&) = delete;
		PlayerComponent& operator=(PlayerComponent&&) = delete;

	public:
		TransformComponent* GetTransform() const { return trans; }
		SpriteComponent* GetSpriteComponent() const { return sprite; }
		ColliderComponent* GetColliderComponent() const { return collider; }
		AnimationComponent* GetAnimationComponent() const { return animation; }
		void SetSpeed(float speed_) { speed = speed_; }
		void SetDirection(const Engine::Maths::Vec2& dir) { direct = dir; }
		void SetDirection(Engine::Maths::Vec2&& dir) { direct = dir; }
		float GetSpeed() const { return speed; }
		Engine::Maths::Vec2 GetTo(float);
		void SetState(std::unique_ptr<State::PlayerState> new_state);
		void SetIsSlow(bool _slow) { is_slow = _slow; }
		bool GetIsSlow() const { return is_slow; }
		

	private:
		void Init() override;
		void HandleInput(Context&) override;
		void Update(float, Context&) override;
	};
}