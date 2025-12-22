#pragma once
#include "../prefix.h"
#include "maths.h"
namespace Engine::Core::Components {
    class AnimationComponent;
}

namespace Engine::Render {
	constexpr SDL_FRect IGNORE_RECT = { 0, 0, 0, 0 };

	struct AnimationFrame {
		SDL_FRect rect;
		float duration;
		Maths::Vec2 scale = { 1.0f, 1.0f };
		float rotation = 0.0f;	//采用Deg
		AnimationFrame()
			: rect({ 0,0,0,0 }), duration(0), scale({ 1.0f,1.0f }), rotation(0) {
		}

		AnimationFrame(const SDL_FRect& r, float d, const Maths::Vec2& s = { 1.0f, 1.0f }, float rot = 0.0f)
			: rect(r), duration(d), scale(s), rotation(rot) {
		}
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
		void AddFrame(const SDL_FRect& rect_, float dur,
			const Maths::Vec2& scale_, float rotation_);
		const AnimationFrame& GetFrame(float time) const;
		const std::string& GetName() const { return name; }
		float GetTotalDuration() const { return total_duration; }
		bool IsLooping() const { return loop; }
		bool IsEmpty() const { return frames.empty(); }
	};

}