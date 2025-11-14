// Effects.h
#pragma once
#include "Components/EffectComponent.h"
#include "Components/TransformComponent.h"
#include "Components/SpriteComponent.h"
#include "maths.h"

namespace Engine::Render::Effect {

    using namespace Engine::Maths;

    // 1. 抖动（Shake）
    class ShakeEffect : public EffectComponent {
        float amplitude = 0.f;
        Vec2  original_pos{ 0,0 };

    public:
        void Init() override {
            auto tr = owner->GetComponent<TransformComponent>();
            if (tr) original_pos = tr->GetPosition();
        }

        void Configure(float amp) { amplitude = amp; }

        void Apply(float t, float dt, Context&) override {
            float decay = 1.f - t * t * t;
            float phase = t * 25.f * 6.283185f;
            float x = sinf(phase) * 0.7f + sinf(phase * 2.3f) * 0.3f;
            float y = cosf(phase * 1.2f) * 0.6f + cosf(phase * 3.1f) * 0.4f;

            Vec2 offset{ amplitude * decay * x, amplitude * decay * y };

            auto tr = owner->GetComponent<TransformComponent>();
            if (tr) tr->SetPosition(original_pos + offset);
        }
    };

    // 2. 红闪（Flash）
    class FlashEffect : public EffectComponent {
        SDL_Color flash_color{ 255,100,100,255 };
        SDL_Color normal_color{ 255,255,255,255 };

    public:
        void Configure(const SDL_Color& flash, int flashes) {
            flash_color = flash;
            float per = duration / (flashes * 2.0f);
        }

        void Apply(float t, float, Context&) override {
            float wave = sinf(t * 6.283185f * 3.0f);
            wave = (wave + 1.0f) * 0.5f;               // 0~1
            SDL_Color cur;
            cur.r = static_cast<Uint8>(normal_color.r + (flash_color.r - normal_color.r) * wave);
            cur.g = static_cast<Uint8>(normal_color.g + (flash_color.g - normal_color.g) * wave);
            cur.b = static_cast<Uint8>(normal_color.b + (flash_color.b - normal_color.b) * wave);
            cur.a = 0;

            auto spr = owner->GetComponent<SpriteComponent>();
            if (spr) {
                spr->SetColorMod(cur);
            }
        }
    };

    // 3. 缩放脉冲（Pulse）
    class PulseEffect : public EffectComponent {
        float min_scale = 0.9f, max_scale = 1.1f;
        Vec2  base_scale{ 1,1 };

    public:
        void Init() override {
            auto spr = owner->GetComponent<SpriteComponent>();
            if (spr) base_scale = spr->GetScale();
        }

        void Configure(float min_s, float max_s) { min_scale = min_s; max_scale = max_s; }

        void Apply(float t, float, Context&) override {
            float s = min_scale + (max_scale - min_scale) *
                (0.5f + 0.5f * sinf(t * 6.283185f));
            Vec2 scale{ base_scale.x * s, base_scale.y * s };

            auto spr = owner->GetComponent<SpriteComponent>();
            if (spr) spr->SetScale(scale);
        }
    };

    // 4. 渐隐（FadeOut） / 渐显（FadeIn）
    class FadeEffect : public EffectComponent {
        bool fade_in = false;
        Uint8 start_alpha = 255, end_alpha = 0;

    public:
        void Configure(bool in, Uint8 from = 0, Uint8 to = 255) {
            fade_in = in;
            start_alpha = from; end_alpha = to;
        }

        void Apply(float t, float, Context&) override {
            Uint8 a = static_cast<Uint8>(start_alpha + (end_alpha - start_alpha) * t);
            if (!fade_in) a = static_cast<Uint8>(start_alpha - (start_alpha - end_alpha) * t);

            auto spr = owner->GetComponent<SpriteComponent>();
            if (spr) {
                SDL_Color c = spr->GetColorMod();
                c.a = a;
                spr->SetColorMod(c);
            }
        }
    };

}