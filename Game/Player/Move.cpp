#include "Move.h"
#include "Idel.h"
#include "../../Engine/context.h"
#include "../../Engine/InputManager.h"
#include "../../Engine/Components/PlayerComponent.h"
#include "../../Engine/Components/TransformComponent.h"
#include "../../Engine/Components/AnimationComponent.h"
namespace Game::Components::State::Player {
	void MoveState::Enter()
	{

	}

	void MoveState::Exit()
	{

	}

	std::unique_ptr<Engine::Core::Components::State::PlayerState> MoveState::HandleInput(Context& ctx)
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

		auto anim_component = player_component->GetAnimationComponent();
		std::string current_anim_name = anim_component->GetCurrentAnimationName();
		if (direction.x < 0.0f) {
			if (current_anim_name != "left" && current_anim_name != "move_left") {
				anim_component->PlayAnimation("left", [anim_component]() {
					anim_component->PlayAnimation("move_left");
					});
			}
		}
		else if (direction.x > 0.0f) {
			if (current_anim_name != "right" && current_anim_name != "move_right") {
				anim_component->PlayAnimation("right", [anim_component]() {
					anim_component->PlayAnimation("move_right");
					});
			}
		}
		else {
			return std::make_unique<IdleState>(player_component);
		}


		return nullptr;
	}

	std::unique_ptr<Engine::Core::Components::State::PlayerState> MoveState::Update(float d_t, Context& ctx)
	{
		auto trans = player_component->GetTransform();
		trans->Translate(player_component->GetTo(d_t));
		player_component->SetDirection({ 0,0 });
		return nullptr;
	}
}