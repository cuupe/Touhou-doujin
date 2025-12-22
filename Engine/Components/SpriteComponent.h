#pragma once
#include "../GameObject.h"
#include "../resources.h"
#include "../Component.h"
#include "../maths.h"
#include "../render.h"
#include "../sprite.h"
#include "TransformComponent.h"
using namespace Engine::Maths;
using namespace Engine::Resource;
using namespace Engine::Render;
namespace Engine::Core::Components {
	class SpriteComponent final :public Component {
		friend class Engine::Core::GameObject;
	private:
		ResourceManager* res = nullptr;
		TransformComponent* trans = nullptr;

		Sprite s;
		Align align;
		SDL_FRect rect;
		Vec2 offset;
		Vec2 size;
		Vec2 scale;
		SDL_Color color_mod;
		float angle;
		bool is_hidden = false;
		bool is_filpped = false;

	public:
		SpriteComponent(
			const std::string& sprite_name,
			ResourceManager& r, std::optional<SDL_FRect> sr = std::nullopt,
			Align a = Align::NONE, const Vec2& _scale = { 1.0f, 1.0f },
			const SDL_Color& _color_mod = { 255, 255, 255, 255 }, float _angle = 0.0f,
			bool i_f = false);
		~SpriteComponent() override = default;
		SpriteComponent(const SpriteComponent&) = delete;
		SpriteComponent& operator=(const SpriteComponent&) = delete;
		SpriteComponent(SpriteComponent&&) = delete;
		SpriteComponent& operator=(SpriteComponent&&) = delete;

	public:
		void UpdateOffset();

	public:
		const Sprite& GetSprite() const { return s; }
		const std::string& GetSpriteTextureName() const { return s.GetTextureName(); }
		void SetSprite(const Sprite& newSprite) { s = newSprite; }
		void SetSprite(const std::string& t_n,
			const std::optional<SDL_FRect>& s_r = std::nullopt,
			bool i_f = false);
		float GetRotation() const { return angle; }
		void SetRotation(float new_angle) { angle = new_angle; }
		const SDL_Color& GetColorMod() const { return color_mod; }
		void SetColorMod(const SDL_Color& color) { color_mod = color; }
		Align GetAlignment() const { return align; }
		void SetAlignment(Align new_alignment);
		const SDL_FRect& GetRect() const { return rect; }
		void SetRect(const SDL_FRect& rc) { rect = rc; }
		const Vec2& GetScale() const { return scale; }
		void SetScale(const Vec2& new_scale) { scale = new_scale; UpdateOffset(); }
		const Vec2& GetOffset() const { return offset; }
		void SetOffset(const Vec2& new_offset) { offset = new_offset; }
		bool IsHidden() const { return is_hidden; }
		void SetHidden(bool i_h) { is_hidden = i_h; }
		bool IsFilpped() const { return is_filpped; }
		void SetFilpped(bool i_f) { is_filpped = i_f; }


	private:
		void UpdateSpriteSize();

	private:
		void Init() override;
		void Update(float, Context&) override {}
		void Render(Context&) override;

	};
}