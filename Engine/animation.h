#pragma once
#include <SDL3/SDL_rect.h>
#include <vector>
#include <string>

namespace Engine::Render {
	struct AnimationFrame {
		SDL_FRect rect;
		float duration;
	};

	class Animation final {
	private:
		std::string name;
		std::vector<AnimationFrame> frames;
		float total_duration = 0.0f;
		bool loop = true;

	public:
		Animation(const std::string& name_ = "", bool loop_ = true);
		~Animation() = default;

		void AddFrame(const SDL_FRect& rect_, float dur);
		const AnimationFrame& GetFrame(float time) const;

		const std::string& GetName() const { return name; }
		float GetTotalDuration() const { return total_duration; }
		bool IsLooping() const { return loop; }
		bool IsEmpty() const { return frames.empty(); }
	};
}