#include "../prefix.h"
#include "animation.h"
namespace Engine::Render {
	Animation::Animation(const std::string& name_, bool loop_)
	:name(name_), loop(loop_){ }

	void Animation::AddFrame(const SDL_FRect& rect_, float dur)
	{
		if (dur <= 0.0f) {
			spdlog::error("错误的持续时间");
		}
		frames.push_back(AnimationFrame{ rect_, dur });
		total_duration += dur;

	}

	void Animation::AddFrame(const SDL_FRect& rect_, float dur,
		const Maths::Vec2& scale_,
		float rotation_,
		const SDL_Color& color_,
		AnimationFrame::Curve curve_,
		const Maths::Vec2& offset_) {
		if (dur <= 0.0f) {
			spdlog::error("错误的持续时间");
			return;
		}
		frames.push_back(AnimationFrame(rect_, dur, scale_, rotation_, color_, curve_, offset_));
		total_duration += dur;
	}

	const AnimationFrame& Animation::GetFrame(float time) const
	{
		if (frames.empty()) {
			spdlog::error("不包含帧序列");
			return {};
		}

		float current_time = time;
		if (loop && total_duration > 0.0f) {
			current_time = std::fmod(time, total_duration);
		}
		else {
			if (current_time >= total_duration) {
				return frames.back();
			}
		}

		float accumulated_time = 0.0f;
		for (const auto& frame : frames) {
			accumulated_time += frame.duration;
			if (current_time < accumulated_time) {
				return frame;
			}
		}

		return frames.back();
	}

	AnimationFrame Animation::GetInterpolatedFrame(float time) const
	{
		if (frames.empty()) {
			spdlog::error("不包含帧序列");
			return {};
		}

		float current_time = time;
		if (loop && total_duration > 0.0f) {
			current_time = std::fmod(time, total_duration);
		}
		else if (current_time >= total_duration) {
			return frames.back();
		}

		float accumulated_time = 0.0f;
		for (size_t i = 0; i < frames.size(); ++i) {
			accumulated_time += frames[i].duration;
			if (current_time < accumulated_time) {
				if (i + 1 >= frames.size()) {
					return frames[i];
				}

				const auto& f1 = frames[i];
				const auto& f2 = frames[i + 1];

				float segment_start = accumulated_time - f1.duration;
				float t = (current_time - segment_start) / f1.duration;
				t = std::clamp(t, 0.0f, 1.0f);
				t = ApplyCurve(t, f1.curve);

				AnimationFrame result = f1;
				result.scale = Maths::Lerp(f1.scale, f2.scale, t);
				result.rotation = Maths::Lerp(f1.rotation, f2.rotation, t);
				result.color_mod.r = static_cast<Uint8>(Maths::Lerp(f1.color_mod.r, f2.color_mod.r, t));
				result.color_mod.g = static_cast<Uint8>(Maths::Lerp(f1.color_mod.g, f2.color_mod.g, t));
				result.color_mod.b = static_cast<Uint8>(Maths::Lerp(f1.color_mod.b, f2.color_mod.b, t));
				result.color_mod.a = static_cast<Uint8>(Maths::Lerp(f1.color_mod.a, f2.color_mod.a, t));
				result.rect = f1.rect;
				return result;
			}
		}
		return frames.back();
	}

	float Animation::ApplyCurve(float t, AnimationFrame::Curve c)
	{
		switch (c) {
		case AnimationFrame::Curve::EASE_IN:     
			return t * t;
		case AnimationFrame::Curve::EASE_OUT:    
			return 1.0f - (1.0f - t) * (1.0f - t);
		case AnimationFrame::Curve::EASE_IN_OUT:
			return t < 0.5f ? 2.0f * t * t
				: 1.0f - std::pow(-2.0f * t + 2.0f, 2.0f) / 2.0f;
		default:
			return t;
		}
	}
}

