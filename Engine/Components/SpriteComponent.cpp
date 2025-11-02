#include "SpriteComponent.h"
#include "../context.h"
#include "ColliderComponent.h"
namespace Engine::Core::Components {
    SpriteComponent::SpriteComponent(
        const std::string& t_name, 
        ResourceMannager& r, std::optional<SDL_FRect> sr,
        Align a, bool i_f)
        :res(&r), s(t_name, sr, i_f), align(a)
    {
        if (!res) {
            spdlog::critical("创建的精灵图组件无法获取资源管理器对象");
        }
        if (sr.has_value()) {
            rect = sr.value();
        }
        else {
            rect = SDL_FRect{ 0,0, (float)(res->GetTexture(t_name)->width), 
                (float)(res->GetTexture(t_name)->height) };
        }

        spdlog::trace("创建SpriteComponent成功{}", t_name);
    }

    void SpriteComponent::UpdateOffset()
    {
        if (size.x <= 0 || size.y <= 0) {
            offset = { 0.0f, 0.0f };
            return;
        }

        Vec2 scale = trans->GetScale();
        switch (align) {
        case Align::TOP_LEFT:      offset = Vec2{ 0.0f, 0.0f } *scale; break;
        case Align::TOP_CENTER:    offset = Vec2{ -size.x / 2.0f, 0.0f } *scale; break;
        case Align::TOP_RIGHT:     offset = Vec2{ -size.x, 0.0f } *scale; break;
        case Align::CENTER_LEFT:   offset = Vec2{ 0.0f, -size.y / 2.0f } *scale; break;
        case Align::CENTER:        offset = Vec2{ -size.x / 2.0f, -size.y / 2.0f } *scale; break;
        case Align::CENTER_RIGHT:  offset = Vec2{ -size.x, -size.y / 2.0f } *scale; break;
        case Align::BOTTOM_LEFT:   offset = Vec2{ 0.0f, -size.y } *scale; break;
        case Align::BOTTOM_CENTER: offset = Vec2{ -size.x / 2.0f, -size.y } *scale; break;
        case Align::BOTTOM_RIGHT:  offset = Vec2{ -size.x, -size.y } *scale; break;
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
        const Vec2& scale = trans->GetScale();
        float angles = trans->GetRotation();

        ctx.GetRenderer().DrawSprite(s, pos, scale, rect, angles);
    }
}