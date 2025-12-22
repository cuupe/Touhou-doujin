#pragma once
#include "../prefix.h"
namespace Engine::Render {
	class Sprite final {
	private:
		std::string tex_name;
		std::optional<SDL_FRect> source_rect;
		bool is_flipped;

	public:
		Sprite(const std::string& t_n,
			const std::optional<SDL_FRect>& s_r = std::nullopt,
			bool i_f = false) :
			tex_name(t_n), source_rect(s_r), is_flipped(i_f) {
		}

	public:
		const std::string& GetTextureName() const { return tex_name; }
		const std::optional<SDL_FRect>& GetSourceRect() const { return source_rect; }
		bool IsFlipped() const { return is_flipped; }

		void SetTextureName(const std::string& t_n) { tex_name = t_n; }
		void SetSourceRect(const std::optional<SDL_FRect>& s_r) { source_rect = s_r; }
		void SetFlipped(bool f) { is_flipped = f; }
	};
}