#include "Move.h"
#include "Idel.h"
#include "../../Engine/context.h"
#include "../../Engine/InputManager.h"
#include "../../Engine/Components/PlayerComponent.h"
#include "../../Engine/Components/TransformComponent.h"
namespace Game::Components::State::Player {
	void IdleState::Enter()
	{
		PlayAnimation("idle");
	}

	void IdleState::Exit()
	{

	}

	std::unique_ptr<Engine::Core::Components::State::PlayerState> IdleState::HandleInput(Context& ctx)
	{
		auto& input = ctx.GetInputManager();
		auto sprite = player_component->GetSpriteComponent();

		if (input.IsActionDown("slow")) {
			player_component->SetIsSlow(true);
		}
		else {
			player_component->SetIsSlow(false);
		}

		Vec2 direction = { 0.0f, 0.0f };

		if (input.IsActionDown("move_forward")) {
			direction.y -= 1.0f;
		}
		if (input.IsActionDown("move_back")) {
			direction.y += 1.0f;
		}
		if (input.IsActionDown("move_right")) {
			direction.x += 1.0f;
		}
		if (input.IsActionDown("move_left")) {
			direction.x -= 1.0f;
		}
		player_component->SetDirection(direction);

		if (direction.x != 0.0f) {
			return std::make_unique<MoveState>(player_component);
		}

			

		return nullptr;
	}

	std::unique_ptr<Engine::Core::Components::State::PlayerState> IdleState::Update(float d_t, Context& ctx)
	{
		auto trans = player_component->GetTransform();
		trans->Translate(player_component->GetTo(d_t));
		player_component->SetDirection({ 0,0 });
		return nullptr;
	}


}