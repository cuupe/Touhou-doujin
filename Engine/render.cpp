#include "render.h"
#include "resources.h"
namespace Engine::Render {
	Renderer::Renderer(SDL_Renderer* s_r, ResourceMannager* r_s)
		:renderer(s_r), res(r_s)
	{ }

	std::optional<SDL_FRect> Renderer::GetSpriteSrcRect(const sprite& sprite)
	{
		SDL_Texture* texture = res->GetTexture(sprite.GetTextureName())->texture.get();
		if (!texture) {
			spdlog::error("无法为 ID {} 获取纹理。", sprite.GetTextureName());
			return std::nullopt;
		}

		auto src_rect = sprite.GetSourceRect();
		if (src_rect.has_value()) {
			if (src_rect.value().w <= 0 || src_rect.value().h <= 0) {
				spdlog::error("源矩形尺寸无效，ID: {}", sprite.GetTextureName());
				return std::nullopt;
			}
			return src_rect;
		}
		else { 
			SDL_FRect result = { 0, 0, 0, 0 };
			if (!SDL_GetTextureSize(texture, &result.w, &result.h)) {
				spdlog::error("无法获取纹理尺寸，ID: {}", sprite.GetTextureName());
				return std::nullopt;
			}
			return result;
		}
	}

	void Renderer::SetDrawColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a)
	{
		if (!SDL_SetRenderDrawColor(renderer, r, g, b, a)) {
			spdlog::error("设置渲染绘制颜色失败：{}", SDL_GetError());
		}
	}

	void Renderer::SetDrawColorFloat(float r, float g, float b, float a)
	{
		if (!SDL_SetRenderDrawColorFloat(renderer, r, g, b, a)) {
			spdlog::error("设置渲染绘制颜色失败：{}", SDL_GetError());
		}
	}


	void Renderer::ClearScreen()
	{
		if (!SDL_RenderClear(renderer)) {
			spdlog::error("清除渲染器失败：{}", SDL_GetError());
		}
	}

	void Renderer::Present()
	{
		SDL_RenderPresent(renderer);
	}

	//vector为二维向量
	void Renderer::DrawSprite(const sprite& sprite, const Pos2& pos, const SpriteRect& s_rc, double angle)
	{
		auto texture = res->GetTexture(sprite.GetTextureName())->texture.get();
		if (!texture) {
			spdlog::error("无法获取纹理：{}", sprite.GetTextureName());
			return;
		}

		auto s_r = GetSpriteSrcRect(sprite);
		if (!s_r.has_value()) {
			spdlog::error("无法获取精灵的源矩形，ID: {}", sprite.GetTextureName());
			return;
		}
		s_r->w = s_rc.w;
		s_r->h = s_rc.h;
		s_r->x = s_rc.x;
		s_r->y = s_rc.y;


		float s_w = s_r->w * s_rc.scale_x;
		float s_h = s_r->h * s_rc.scale_y;

		SDL_FRect d_r = {
			pos.x,
			pos.y,
			s_w,
			s_h
		};

		//TODO:可能添加更多约束


		if (!SDL_RenderTextureRotated(renderer,
			texture, &s_r.value(), &d_r, angle, nullptr, sprite.IsFlipped() ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE)) {
			spdlog::error("渲染旋转纹理失败（ID: {}）：{}", sprite.GetTextureName(), SDL_GetError());
		}
	}

	void Renderer::DrawUI(const sprite& sprite, const Pos2& pos, const SpriteRect& s_rc)
	{
		auto texture = res->GetTexture(sprite.GetTextureName())->texture.get();
		if (!texture) {
			spdlog::error("无法为 ID {} 获取纹理。", sprite.GetTextureName());
			return;
		}

		auto s_r = GetSpriteSrcRect(sprite);
		if (!s_r.has_value()) {
			spdlog::error("无法获取精灵的源矩形，ID: {}", sprite.GetTextureName());
			return;
		}

		SDL_FRect d_r = { pos.x, pos.y, 0, 0};

		d_r.w = texture->w;
		d_r.h = texture->h;


		// 执行绘制(未考虑UI旋转)
		if (!SDL_RenderTextureRotated(renderer, texture, &s_r.value(), &d_r, 0.0, nullptr, sprite.IsFlipped() ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE)) {
			spdlog::error("渲染 UI Sprite 失败 (ID: {}): {}", sprite.GetTextureName(), SDL_GetError());
		}
	}
}