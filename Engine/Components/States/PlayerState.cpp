#include "PlayerState.h"
#include "../PlayerComponent.h"
#include "../AnimationComponent.h"
#include "../prefix.h"
namespace Engine::Core::Components::State {
	void PlayerState::PlayAnimation(const std::string& name)
	{
		if (!player_component) {
			spdlog::error("未找到玩家组件，无法播放");
			return;
		}

		auto animation_component = player_component->GetAnimationComponent();
		if (!animation_component) {
			spdlog::error("未找到动画组件");
			return;
		}
		animation_component->PlayAnimation(name);
	}
}