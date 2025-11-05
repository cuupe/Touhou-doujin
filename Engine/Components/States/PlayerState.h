#pragma once
#include <memory>
#include <string>

namespace Engine::Core {
	class Context;
}
using namespace Engine::Core;
namespace Engine::Core::Components {
	class PlayerComponent;
}
using namespace Engine::Core::Components;
namespace Engine::Core::Components::State {
	class PlayerState {
		friend class PlayerComponent;
	
	protected:
		PlayerComponent* player_component = nullptr;

	public:
		PlayerState(PlayerComponent* p_c):
			player_component(p_c){ }
		virtual ~PlayerState() = default;
		PlayerState(const PlayerState&) = delete;
		PlayerState& operator=(const PlayerState&) = delete;
		PlayerState(PlayerState&&) = delete;
		PlayerState& operator=(PlayerState&&) = delete;

	public:
		void PlayAnimation(const std::string& name);

	protected:
		virtual void Enter() = 0;
		virtual void Exit() = 0;
		virtual std::unique_ptr<PlayerState> HandleInput(Context&) = 0;
		virtual std::unique_ptr<PlayerState> Update(float, Context&) = 0;
	};
}