#pragma once
#include "../component.h"
#include <string>
#include <unordered_map>
#include <memory>
namespace Engine::Render {
	class Animation;
}
using namespace Engine::Render;
namespace Engine::Core::Components {
	class SpriteComponent;
}
namespace Engine::Core::Components {
	class AnimationComponent final :public Component {
		friend class GameObject;

	private:
		std::unordered_map<std::string, std::unique_ptr<Animation>> animations;
		SpriteComponent* sprite = nullptr;
		Animation* current_animation = nullptr;
		float animation_timer = 0.0f;
		bool is_playing = false;
		bool is_finished_and_remove = false;

	public:
		void AddAnimation(std::unique_ptr<Animation> animation);
		void StopAnimation() { is_playing = false; }
		void PlayAnimaiton(const std::string& name);
		void StopAnimaiton();
		std::string GetCurrentAnimationName() const;
		bool IsAnimationFinished() const;
		bool IsFinishedAndRemove() const { return is_finished_and_remove; }
		void SetFinishedAndRemove(bool want) { is_finished_and_remove = want; }

	private:
		void Init() override;
		void Update(float, Context&) override;
	};
}