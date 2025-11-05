#include "Slow.h"
namespace Game::Components::State::Player {
	void SlowState::Enter()
	{
		PlayAnimation("slow");
	}

	void SlowState::Exit()
	{

	}

	std::unique_ptr<Engine::Core::Components::State::PlayerState> SlowState::HandleInput(Context& ctx)
	{
		return nullptr;
	}
	std::unique_ptr<Engine::Core::Components::State::PlayerState> SlowState::Update(float d_t, Context& ctx)
	{

		//TODO
		return nullptr;
	}
}