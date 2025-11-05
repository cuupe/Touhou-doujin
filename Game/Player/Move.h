#pragma once
#include "../../Engine/Components/States/PlayerState.h"
namespace Game::Components::State::Player {
	class MoveState final :public Engine::Core::Components::State::PlayerState {
		friend class Engine::Core::Components::PlayerComponent;

	private:


	public:
		MoveState(PlayerComponent* player_component) :
			PlayerState(player_component) {
		}
		~MoveState() override = default;

	private:
		void Enter() override;
		void Exit() override;
		std::unique_ptr<PlayerState> HandleInput(Context&) override;
		std::unique_ptr<PlayerState> Update(float, Context&) override;
	};
}