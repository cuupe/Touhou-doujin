#pragma once
#include "../prefix.h"
namespace Engine::Resource {
	class ResourceMannager;
}
using namespace Engine::Resource;

namespace Engine::Render {
	//精灵图的绘图区域
	struct SpriteRect {
		float x;
		float y;
		float h;
		float w;
		float scale_x;
		float scale_y;
		SpriteRect(float x, float y, float w, float h) :
			x(x), y(y),
			h(h), w(w),
			scale_x(1),
			scale_y(1) {
		}

		SpriteRect(float x, float y, float w, float h, float scale_x, float scale_y) :
			x(x), y(y),
			h(h), w(w),
			scale_x(scale_x),
			scale_y(scale_y) {
		}
	};

	struct Pos2 {
		float x;
		float y;

		Pos2():
			x(0),y(0){ }
		Pos2(float x) :
			x(x), y(0) {
		}
		Pos2(float x, float y) :
			x(x), y(y) {
		}
	};



	class sprite final {
	private:
		std::string tex_name;
		std::optional<SDL_FRect> source_rect;
		bool is_flipped;

	public:
		sprite(const std::string& t_n,
			const std::optional<SDL_FRect>& s_r = std::nullopt,
			bool i_f = false):
			tex_name(t_n),source_rect(s_r),is_flipped(i_f){ }

	public:
		const std::string& GetTextureName() const { return tex_name; }
		const std::optional<SDL_FRect>& GetSourceRect() const { return source_rect; }
		bool IsFlipped() const { return is_flipped; }

		void SetTextureName(const std::string& t_n) { tex_name = t_n; }
		void SetSourceRect(const std::optional<SDL_FRect>& s_r) { source_rect = s_r; }
		void SetFlipped(bool f) { is_flipped = f; }
	};

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
		std::optional<SDL_FRect> GetSpriteSrcRect(const sprite& sprite);
		//bool isRectInViewport(const SDL_FRect& rect);
		void SetDrawColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a);
		void SetDrawColorFloat(float r, float g, float b, float a);
		void ClearScreen();
		void Present();

		//2D部分
	public:
		void DrawSprite(const sprite& sprite, const Pos2& pos, const SpriteRect& s_rc, double angle = 0.0f);

		void DrawUI(const sprite& sprite, const Pos2& pos,
			const SpriteRect& s_rc);

		//3D部分
	public:

	};

}