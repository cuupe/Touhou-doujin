#include "AnimationComponent.h"
#include "SpriteComponent.h"
#include "../animation.h"

namespace Engine::Core::Components {
	void AnimationComponent::AddAnimation(std::unique_ptr<Animation> animation) {
		if (!animation) {
			return;
		}

		std::string name = animation->GetName();
		animations[name] = std::move(animation);
	}

	void AnimationComponent::RemoveAllAnimations()
	{
		animations.clear();
	}

	void AnimationComponent::RemoveAnimation(const std::string& anim_name)
	{
		animations.erase(anim_name);
	}

	void AnimationComponent::PlayAnimation(const std::string& name, std::function<void()> on_finished)
	{
		auto it = animations.find(name);
		if (it == animations.end()) {
			spdlog::error("找不到对应的序列帧");
			return;
		}

		if (current_animation == it->second.get() && is_playing) {
			if (on_finished && !current_animation->IsLooping()) {
				on_animation_finished_callback = on_finished;
			}
			else {
				on_animation_finished_callback = nullptr;
			}
			return;
		}

		on_animation_finished_callback = nullptr;

		if (on_finished) {
			if (!it->second.get()->IsLooping()) {
				on_animation_finished_callback = on_finished;
			}
			else {
				spdlog::warn("尝试为循环动画 '{}' 设置完成回调，已忽略。", name);
			}
		}

		current_animation = it->second.get();
		animation_timer = 0.0f;
		is_playing = true;

		if (sprite && !current_animation->IsEmpty()) {
			const auto& first_frame = current_animation->GetFrame(0.0f);
			sprite->SetRect(first_frame.rect);
		}
	}

	void AnimationComponent::StopAnimaiton() {
		is_playing = false;
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
		AnimationFrame current_frame = current_animation->GetFrame(animation_timer);
		sprite->SetRect(current_frame.rect);
		sprite->SetScale(current_frame.scale);
		sprite->SetRotation(current_frame.rotation);
		if (!current_animation->IsLooping() &&
			animation_timer >= current_animation->GetTotalDuration()) {

			animation_timer = current_animation->GetTotalDuration();
			is_playing = false;


			if (on_animation_finished_callback) {
				auto callback = on_animation_finished_callback;
				on_animation_finished_callback = nullptr;
				callback();
			}

			if (!is_playing && is_finished_and_remove) {
				owner->SetNeedRemove(true);
			}
		}
		else if (current_animation->IsLooping()) {
			float duration = current_animation->GetTotalDuration();
			if (animation_timer >= duration) {
				animation_timer = fmodf(animation_timer, duration);
			}
		}
	}



}