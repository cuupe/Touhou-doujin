#pragma once
#include "../component.h"
#include <string>
#include <unordered_map>
#include <memory>
#include <functional>
namespace Engine::Render {
	class Animation;
	class AnimationSequence;
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
		std::function<void()> on_animation_finished_callback = nullptr;

	public:
		void AddAnimation(std::unique_ptr<Animation> animation);
		void RemoveAllAnimations();
		void RemoveAnimation(const std::string& anim_name);
		void StopAnimation() { is_playing = false; }
		void PlayAnimation(const std::string& name, std::function<void()> on_finished = nullptr);
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