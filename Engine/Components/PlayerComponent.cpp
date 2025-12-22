#include "PlayerComponent.h"
#include "TransformComponent.h"
#include "SpriteComponent.h"
#include "ColliderComponent.h"
#include "AnimationComponent.h"


#include "../../Game/Player/Idel.h"
#include "../../Game/Player/Move.h"


namespace Engine::Core::Components {
	PlayerComponent::PlayerComponent(float speed_)
		:speed(speed_)
	{ }

	Vec2 PlayerComponent::GetTo(float d_t)
	{
		direct.Normalized();
		if (is_slow) {
			return speed * d_t * direct * 0.5f;
		}
		return speed * d_t * direct;
	}

	void PlayerComponent::SetState(std::unique_ptr<State::PlayerState> new_state)
	{
		if (!new_state) {
			spdlog::warn("不允许设置空状态");
			return;
		}
		if (current_state) {
			current_state->Exit();
		}
		current_state = std::move(new_state);
		current_state->Enter();
	}

	void PlayerComponent::Init()
	{
		if (!owner) {
			spdlog::error("PlayerComponent没有挂载游戏对象");
			return;
		}

		trans = owner->GetComponent<TransformComponent>();
		sprite = owner->GetComponent<SpriteComponent>();
		collider = owner->GetComponent<ColliderComponent>();
		animation = owner->GetComponent<AnimationComponent>();
		if (!trans || !sprite || !collider || !animation) {
			spdlog::error("Player对象缺少组件");
		}

		current_state = std::make_unique<Game::Components::State::Player::IdleState>(this);
		if (current_state) {
			SetState(std::move(current_state));
		}
		else {
			spdlog::error("无法获取初始状态");
		}
	}

	void PlayerComponent::HandleInput(Context& ctx)
	{
		if (!current_state) {
			return;
		}

		auto next_state = current_state->HandleInput(ctx);
		if (next_state) {
			SetState(std::move(next_state));
		}
	}

	void PlayerComponent::Update(float d_t, Context& ctx)
	{
		if (!current_state) {
			return;
		}

		auto next_state = current_state->Update(d_t, ctx);
		if (next_state) {
			SetState(std::move(next_state));
		}
	}
}