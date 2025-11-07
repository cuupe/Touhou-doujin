#pragma once
#include "../prefix.h"
namespace Engine::Resource {
	class ResourceMannager;
}
namespace Engine::Maths {
	struct SpriteRect;
	struct Vec2;
}
using namespace Engine::Resource;
using namespace Engine::Maths;
namespace Engine::Render {
	class Sprite;

	class Renderer final{
	private:
		SDL_Renderer* renderer = nullptr;
		ResourceMannager* res = nullptr;

	public:
		Renderer(SDL_Renderer* s_r, ResourceMannager* r_s);
		~Renderer() = default;
		Renderer(const Renderer&) = delete;
		Renderer& operator=(const Renderer&) = delete;
		Renderer(Renderer&&) = delete;
		Renderer& operator=(Renderer&&) = delete;

	public:
		SDL_Renderer* getSDLRenderer() const { return renderer; }

	public:
		std::optional<SDL_FRect> GetSpriteSrcRect(const Sprite& sprite);
		void SetDrawColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a);
		void SetDrawColorFloat(float r, float g, float b, float a);
		void ClearScreen();
		void Present();

		//2D部分
	public:
		void DrawSprite(const Sprite& sprite, const Vec2& pos, 
			const Vec2& scale, const std::optional<SDL_FRect>& s_rc, double angle = 0.0f);

		void DrawUI(const Sprite& sprite, const Vec2& pos,
			const std::optional<SDL_FRect>& s_rc);

		//3D部分
	public:

	};

}