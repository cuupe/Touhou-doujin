#include "ColliderComponent.h"

namespace Engine::Core::Components {
    ColliderComponent::ColliderComponent(std::unique_ptr<Collider::Collider> collider_, 
        Align alignment, bool is_trigger_, bool is_active_):
        collider(std::move(collider_)), align(alignment),
        is_trigger(is_trigger_), is_active(is_active_)
    {
        if (!collider) {
            spdlog::error("创建碰撞组件失败， 碰撞器是空的");
        }
    }

    void ColliderComponent::Init() {
        if (!owner) {
            spdlog::error("组件没有所有者");
            return;
        }
        trans = owner->GetComponent<TransformComponent>();
        if (!trans) {
            spdlog::error("碰撞器未获取到Transform组件");
            return;
        }

        UpdateOffset();
    }

    void ColliderComponent::SetAlignment(Align al) {
        align = al;
        if (trans && collider) {
            UpdateOffset();
        }
    }

    void ColliderComponent::UpdateOffset() {
        if (!collider) {
            return;
        }

        auto collider_size = collider->Get_AABB_Size();

        if (collider_size.x <= 0.0f || collider_size.y <= 0.0f) {
            offset = { 0.0f, 0.0f };
            return;
        }
        auto scale = trans->GetScale();

        switch (align) {
        case Align::TOP_LEFT:
            offset = Vec2{ 0.0f, 0.0f } * scale;
            break;
        case Align::TOP_CENTER:
            offset = Vec2{ -collider_size.x / 2.0f, 0.0f } * scale;
            break;
        case Align::TOP_RIGHT:
            offset = Vec2{ -collider_size.x, 0.0f } * scale;
            break;
        case Align::CENTER_LEFT:
            offset = Vec2{ 0.0f, -collider_size.y / 2.0f } * scale;
            break;
        case Align::CENTER:
            offset = Vec2{ -collider_size.x / 2.0f, -collider_size.y / 2.0f } * scale;
            break;
        case Align::CENTER_RIGHT:
            offset = Vec2{ -collider_size.x, -collider_size.y / 2.0f } * scale;
            break;
        case Align::BOTTOM_LEFT:
            offset = Vec2{ 0.0f, -collider_size.y } * scale;
            break;
        case Align::BOTTOM_CENTER:
            offset = Vec2{ -collider_size.x / 2.0f, -collider_size.y } * scale;
            break;
        case Align::BOTTOM_RIGHT:
            offset = Vec2{ -collider_size.x, -collider_size.y } * scale;
            break;
        default:
            break;
        }
    }



    Rect ColliderComponent::GetWorldAABB() const
    {
        if (!trans || !collider) {
            return { 0.0f, 0.0f, 0.0f, 0.0f };
        }

        const Vec2 top_left = trans->GetPosition() + offset;
        const Vec2 base_size = collider->Get_AABB_Size();
        const Vec2 scale = trans->GetScale();
        Vec2 scaled_size = base_size * scale;
        return { top_left, scaled_size };
    }


}