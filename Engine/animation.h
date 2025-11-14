#pragma once
#include <SDL3/SDL_rect.h>
#include <vector>
#include <string>
#include "maths.h"

namespace Engine::Render {
	constexpr SDL_FRect IGNORE_RECT = { 0, 0, 0, 0 };

	struct AnimationFrame {
		SDL_FRect rect;
		float duration;
		Maths::Vec2 scale = { 1.0f, 1.0f };
		float rotation = 0.0f;	//采用Deg
		SDL_Color color_mod = { 255, 255, 255, 255 };
		enum class Curve { LINEAR, EASE_IN, EASE_OUT, EASE_IN_OUT };
		Curve curve = Curve::LINEAR;
		Maths::Vec2 offset = { 0.0f, 0.0f };
		AnimationFrame()
			: rect({ 0,0,0,0 }), duration(0), scale({ 1.0f,1.0f }), rotation(0),
			color_mod({ 255,255,255,255 }), curve(Curve::LINEAR) {
		}

		AnimationFrame(const SDL_FRect& r, float d, const Maths::Vec2& s = { 1.0f, 1.0f }, float rot = 0.0f,
			const SDL_Color& c = { 255,255,255,255 }, Curve cv = Curve::LINEAR,
			const Maths::Vec2& _offset = { 0.0f, 0.0f })
			: rect(r), duration(d), scale(s), rotation(rot), color_mod(c), curve(cv), offset(_offset){
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
			const Maths::Vec2& scale_, float rotation_,
			const SDL_Color& color_, AnimationFrame::Curve curve_ = AnimationFrame::Curve::LINEAR,
			const Maths::Vec2& offset_ = { 0.0f,0.0f });
		const AnimationFrame& GetFrame(float time) const;
		AnimationFrame GetInterpolatedFrame(float time) const;
		const std::string& GetName() const { return name; }
		float GetTotalDuration() const { return total_duration; }
		bool IsLooping() const { return loop; }
		bool IsEmpty() const { return frames.empty(); }

	private:
		static float ApplyCurve(float t, AnimationFrame::Curve c);
	};
}