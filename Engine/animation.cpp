#include "animation.h"
#include "../prefix.h"
namespace Engine::Render {
	Animation::Animation(const std::string& name_, bool loop_)
	:name(name_), loop(loop_){ }

	void Animation::AddFrame(const SDL_FRect& rect_, float dur)
	{
		if (dur <= 0.0f) {
			spdlog::error("错误的持续时间");
		}
		frames.push_back({ rect_, dur });
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
}