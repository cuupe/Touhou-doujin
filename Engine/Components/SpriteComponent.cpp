#include "SpriteComponent.h"
#include "../context.h"
#include "ColliderComponent.h"
namespace Engine::Core::Components {
    SpriteComponent::SpriteComponent(
        const std::string& sprite_name,
        ResourceManager& r, std::optional<SDL_FRect> sr,
        Align a, const Vec2& _scale, const SDL_Color& _color_mod,
        float _angle, bool i_f)
        :res(&r), s(sprite_name, sr, i_f), align(a), scale(_scale),
        angle(_angle), color_mod(_color_mod)
    {
        if (!res) {
            spdlog::critical("创建的精灵图组件无法获取资源管理器对象");
        }
        if (sr.has_value()) {
            rect = sr.value();
        }
        else {
            rect = SDL_FRect{ 0.0f, 0.0f, (float)(res->GetTexture(sprite_name)->width),
                (float)(res->GetTexture(sprite_name)->height) };
        }

        spdlog::trace("创建SpriteComponent成功{}", sprite_name);
    }

    void SpriteComponent::UpdateOffset()
    {
        if (size.x <= 0 || size.y <= 0) {
            offset = { 0.0f, 0.0f };
            return;
        }

        Vec2 tscale = trans->GetScale() * scale;
        switch (align) {
        case Align::TOP_LEFT:      offset = Vec2{ 0.0f, 0.0f } * tscale; break;
        case Align::TOP_CENTER:    offset = Vec2{ -size.x / 2.0f, 0.0f } * tscale; break;
        case Align::TOP_RIGHT:     offset = Vec2{ -size.x, 0.0f } * tscale; break;
        case Align::CENTER_LEFT:   offset = Vec2{ 0.0f, -size.y / 2.0f } * tscale; break;
        case Align::CENTER:        offset = Vec2{ -size.x / 2.0f, -size.y / 2.0f } * tscale; break;
        case Align::CENTER_RIGHT:  offset = Vec2{ -size.x, -size.y / 2.0f } * tscale; break;
        case Align::BOTTOM_LEFT:   offset = Vec2{ 0.0f, -size.y } * tscale; break;
        case Align::BOTTOM_CENTER: offset = Vec2{ -size.x / 2.0f, -size.y } * tscale; break;
        case Align::BOTTOM_RIGHT:  offset = Vec2{ -size.x, -size.y } * tscale; break;
        case Align::NONE:
        default:                                      break;
        }

    }

    void SpriteComponent::SetSprite(const std::string& t_n, const std::optional<SDL_FRect>& s_r, bool i_f)
    {
        s = std::move(Sprite(t_n, s_r, i_f));
    }

    void SpriteComponent::SetAlignment(Align new_alignment)
    {
        align = new_alignment;
        UpdateOffset();
    }

    void SpriteComponent::UpdateSpriteSize()
    {
        if (!res) {
            spdlog::error("无法获取纹理");
            return;
        }
        if (s.GetSourceRect().has_value()) {
            const SDL_FRect& sr = s.GetSourceRect().value();
            size = { sr.w, sr.h };
        }
        else {
            if (!SDL_GetTextureSize(res->GetTexture(s.GetTextureName())->texture.get(), &size.x, &size.y)) {
                spdlog::error("无法查询纹理尺寸: {}", s.GetTextureName());
                size = { 0, 0 };
            }
        }
    }

    void SpriteComponent::Init()
    {
        if (!owner) {
            spdlog::error("SpriteComponent未挂载");
            return;
        }

        trans = owner->GetComponent<TransformComponent>();
        if (!trans) {
            spdlog::warn("SpriteComponent获取Transform时失效:{}", owner->GetName());
            return;
        }
        UpdateSpriteSize();
        UpdateOffset();
    }

    void SpriteComponent::Render(Context& ctx)
    {
        if (is_hidden || !trans || !res) {
            return;
        }
        const Vec2& pos = trans->GetPosition() + offset;
        const Vec2& _scale = trans->GetScale() * scale;
        float angles = trans->GetRotation() + angle;

        ctx.GetRenderer().DrawSprite(s, pos, _scale, rect, color_mod, angles);
    }
}