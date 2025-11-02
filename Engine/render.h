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
	class Sprite final {
	private:
		std::string tex_name;
		std::optional<SDL_FRect> source_rect;
		bool is_flipped;

	public:
		Sprite(const std::string& t_n,
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