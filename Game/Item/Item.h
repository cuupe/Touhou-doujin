#pragma once
#include "../../Engine/maths.h"
#include "../../prefix.h"

namespace Game::Items {
    using f32 = float;
    using namespace Engine::Maths;
    enum class ItemType {
        BigPower,
        SmallPower,
        QuarterLife,
        FullLife,
        Point,
        Unknown
    };

    enum ItemState {
        IS_UNUSED,
        IS_FALLING,
        IS_MAGNETIZED,
        IS_COLLECTED
    };

    struct ItemConfig {
        std::string source;
        SDL_FRect rect;
        Vec2 scale = { 1.5f, 1.5f };
    };
    struct ItemData {
        ItemType type = ItemType::Unknown;
        ItemState state = IS_UNUSED;
        Vec2 position;
        Vec2 velocity;
        SDL_FRect rect;
        Vec2 hit_box = { 24.0f, 24.0f };
        Vec2 scale;
        f32 magnet_speed = 0.0f;
        f32 timer = 0.0f;
    };

    constexpr int MAX_ITEM_SIZE = 512;
}