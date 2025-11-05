#include "AnimationComponent.h"
#include "SpriteComponent.h"
#include "../animation.h"

namespace Engine::Core::Components {
	void AnimationComponent::AddAnimation(std::unique_ptr<Animation> animation){
		if (!animation) {
			return;
		}

		std::string name = animation->GetName();
		animations[name] = std::move(animation);
	}

	void AnimationComponent::PlayAnimaiton(const std::string& name)
	{
		auto it = animations.find(name);
		if (it == animations.end()) {
			spdlog::error("找不到对应的序列帧");
			return;
		}

		if (current_animation == it->second.get() && is_playing) {
			return;
		}

		current_animation = it->second.get();
		animation_timer = 0.0f;
		is_playing = true;

		if (sprite && !current_animation->IsEmpty()) {
			const auto& first_frame = current_animation->GetFrame(0.0f);
			sprite->SetRect(first_frame.rect);
		}
	}

	std::string AnimationComponent::GetCurrentAnimationName() const
	{
		if (current_animation) {
			return current_animation->GetName();
		}
		return "";
	}

	bool AnimationComponent::IsAnimationFinished() const
	{
		if (!current_animation || current_animation->IsLooping()) {
			return false;
		}
		return animation_timer >= current_animation->GetTotalDuration();
	}

	void AnimationComponent::Init()
	{
		sprite = owner->GetComponent<SpriteComponent>();
		if (!sprite) {
			spdlog::error("AnimationComponent未找到精灵图组件");
		}
	}

	void AnimationComponent::Update(float d_t, Context& ctx)
	{
		if (!is_playing || !current_animation || 
			!sprite || current_animation->IsEmpty()) {
			return;
		}

		animation_timer += d_t;

		const auto& current_frame = current_animation->GetFrame(animation_timer);
		sprite->SetRect(current_frame.rect);
		
		//处理非循环动画结束的逻辑
		if (!current_animation->IsLooping() &&
			animation_timer >= current_animation->GetTotalDuration()) {
			animation_timer = current_animation->GetTotalDuration();
			if (is_finished_and_remove) {
				is_playing = false;
				owner->SetNeedRemove(true);
			}
		}

	}


}