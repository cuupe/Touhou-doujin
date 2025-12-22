#pragma once
#include "Components/EffectComponent.h"
#include "Components/TransformComponent.h"
#include "Components/SpriteComponent.h"
#include "context.h"
#include "render.h"
#include "maths.h"
#ifdef max
#undef max
#endif


namespace Engine::Render::Effect {
    using namespace Engine::Maths;
    
    class ShakeEffect : public EffectComponent {
        float amplitude = 0.f;
        Vec2  original_pos{ 0,0 };

    public:
        void Init() override {
            auto tr = owner->GetComponent<TransformComponent>();
            if (tr) { 
                original_pos = tr->GetPosition(); 
            }
        }

        void OnFinished() override {
            auto tr = owner->GetComponent<TransformComponent>();
            if (tr) {
                tr->SetPosition(original_pos);
            }
        }


        void Configure(float amp) { amplitude = amp; }

        void Apply(float t, float dt, Context&) override {
            float decay = 1.0f - t * t * t;
            float phase = t * 25.f * 6.283185f;
            float x = sinf(phase) * 0.7f + sinf(phase * 2.3f) * 0.3f;
            float y = cosf(phase * 1.2f) * 0.6f + cosf(phase * 3.1f) * 0.4f;

            Vec2 offset{ amplitude * decay * x, amplitude * decay * y };

            auto tr = owner->GetComponent<TransformComponent>();
            if (tr) {
                tr->SetPosition(original_pos + offset);
            }
        }
    };

    class FlashEffect : public EffectComponent {
        SDL_Color flash_color{ 255, 100, 100, 255 };
        SDL_Color normal_color{ 255, 255, 255, 255 };
        float flash_frequency = 16.0f;

    public:
        void Configure(const SDL_Color& flash) {
            flash_color = flash;
        }

        void OnFinished() override {
            auto spr = owner->GetComponent<SpriteComponent>();
            if (spr) {
                spr->SetColorMod(normal_color);
            }
            need_destroy = true;
        }


        void Apply(float t, float, Context&) override {
            float wave = sinf(t * 6.283185f * flash_frequency);
            wave = (wave + 1.0f) * 0.5f;

            SDL_Color cur{
                static_cast<Uint8>(normal_color.r + (flash_color.r - normal_color.r) * wave),
                static_cast<Uint8>(normal_color.g + (flash_color.g - normal_color.g) * wave),
                static_cast<Uint8>(normal_color.b + (flash_color.b - normal_color.b) * wave),
                255
            };

            auto spr = owner->GetComponent<SpriteComponent>();
            if (spr) {
                spr->SetColorMod(cur);
            }
        }
    };


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
            if (!fade_in) {
                a = static_cast<Uint8>(start_alpha - (start_alpha - end_alpha) * t);
            }

            auto spr = owner->GetComponent<SpriteComponent>();
            if (spr) {
                SDL_Color c = spr->GetColorMod();
                c.a = a;
                spr->SetColorMod(c);
            }
        }
    };

    class BlinkEffect : public EffectComponent {
    private:
        float frequency = 3.0f;
        float min_alpha = 70;
        SDL_Color original_color{};

    public:
        void Init() override {
            auto spr = owner->GetComponent<SpriteComponent>();
            if (spr) {
                original_color = spr->GetColorMod();
            }
            duration = 1e9;
        }

        void OnFinished() override {
            auto spr = owner->GetComponent<SpriteComponent>();
            if (spr) {
                spr->SetColorMod(original_color);
            }
        }

        void Apply(float t, float dt, Context&) override {
            float wave = 0.5f + 0.5f * sinf(t * frequency * 6.283185f);

            Uint8 alpha = static_cast<Uint8>(min_alpha + (255 - min_alpha) * wave);
            SDL_Color color = original_color;
            color.a = alpha;


            auto spr = owner->GetComponent<SpriteComponent>();
            if (spr) {
                spr->SetColorMod(color);
            }
        }

        void Configure(float freq = 3.0f, float min_a = 70) {
            frequency = freq;
            min_alpha = static_cast<Uint8>(min_a);
        }
    };

    class SlideEffect : public EffectComponent {
        Vec2 original_pos{ 0, 0 };
        Vec2 offset{ 0, 0 };
    public:
        void Init() override {
            auto tr = owner->GetComponent<TransformComponent>();
            if (tr) {
                original_pos = tr->GetPosition();
                tr->SetPosition(original_pos + offset);
            }
        }
        void OnFinished() override {
            auto tr = owner->GetComponent<TransformComponent>();
            if (tr) {
                tr->SetPosition(original_pos);
            }
        }
        void Configure(Vec2 from_off) { offset = from_off; }
        void Apply(float t, float dt, Context&) override {
            float ease = 1.0f - powf(1.0f - t, 3.0f); // Cubic ease out
            Vec2 current_pos = original_pos + offset * (1.0f - ease);
            auto tr = owner->GetComponent<TransformComponent>();
            if (tr) {
                tr->SetPosition(current_pos);
            }
        }
    };

    class RotateEffect : public EffectComponent {
        float original_rot = 0.0f;
        float start_angle = 0.0f;
    public:
        void Init() override {
            auto tr = owner->GetComponent<TransformComponent>();
            if (tr) {
                original_rot = tr->GetRotation();
                tr->SetRotationDeg(original_rot + start_angle);
            }
        }
        void OnFinished() override {
            auto tr = owner->GetComponent<TransformComponent>();
            if (tr) {
                tr->SetRotationDeg(original_rot);
            }
        }
        void Configure(float from_ang) { start_angle = from_ang; }
        void Apply(float t, float dt, Context&) override {
            float ease = 1.0f - powf(1.0f - t, 3.0f);
            float current_rot = original_rot + start_angle * (1.0f - ease);
            auto tr = owner->GetComponent<TransformComponent>();
            if (tr) {
                tr->SetRotationDeg(current_rot);
            }
        }
    };

    class BounceEffect : public EffectComponent {
        Vec2 base_scale{ 1.0f, 1.0f };
        float amplitude = 1.2f;
    public:
        void Init() override {
            auto spr = owner->GetComponent<SpriteComponent>();
            if (spr) {
                base_scale = spr->GetScale();
                spr->SetScale({ 0.0f, 0.0f });
            }
        }
        void OnFinished() override {
            auto spr = owner->GetComponent<SpriteComponent>();
            if (spr) {
                spr->SetScale(base_scale);
            }
        }
        void Configure(float amp) { amplitude = amp; }
        void Apply(float t, float dt, Context&) override {
            float bounce = amplitude * (1.0f - t) * sinf(t * 6.283185f * 4.0f) * expf(-t * 3.0f) + 1.0f;
            Vec2 scale{ base_scale.x * bounce, base_scale.y * bounce };
            auto spr = owner->GetComponent<SpriteComponent>();
            if (spr) {
                spr->SetScale(scale);
            }
        }
    };

    class ScaleEffect : public EffectComponent {
        Vec2 base_scale{ 1.0f, 1.0f };
        float start_scale = 0.0f;
        float end_scale = 1.0f;
    public:
        void Init() override {
            auto spr = owner->GetComponent<SpriteComponent>();
            if (spr) {
                base_scale = spr->GetScale();
                spr->SetScale({ base_scale.x * start_scale, base_scale.y * start_scale });
            }
        }
        void OnFinished() override {
            auto spr = owner->GetComponent<SpriteComponent>();
            if (spr) {
                spr->SetScale({ base_scale.x * end_scale, base_scale.y * end_scale });
            }
        }
        void Configure(float from_s, float to_s) { start_scale = from_s; end_scale = to_s; }
        void Apply(float t, float dt, Context&) override {
            float ease = 1.0f - powf(1.0f - t, 3.0f);
            float s = start_scale + (end_scale - start_scale) * ease;
            Vec2 scale{ base_scale.x * s, base_scale.y * s };
            auto spr = owner->GetComponent<SpriteComponent>();
            if (spr) {
                spr->SetScale(scale);
            }
        }
    };

    class SlideDirectionEffect : public EffectComponent {
        Vec2 original_pos{ 0,0 };
        Vec2 offset{ 0,0 };
        bool is_slide_in = true;

    public:
        void Init() override {
            auto tr = owner->GetComponent<TransformComponent>();
            if (tr) original_pos = tr->GetPosition();
        }

        void OnFinished() override {
            auto tr = owner->GetComponent<TransformComponent>();
            if (tr) tr->SetPosition(original_pos);
        }

        void Configure(Vec2 off, bool slide_in) {
            offset = off; is_slide_in = slide_in;
            auto tr = owner->GetComponent<TransformComponent>();
            if (tr && !is_slide_in) tr->SetPosition(original_pos + offset);
        }

        void Apply(float t, float, Context&) override {
            float ease = 1.0f - powf(1.0f - t, 3.0f);
            Vec2 current = is_slide_in ? (original_pos + offset * (1.0f - ease)) : (original_pos + offset * ease);
            auto tr = owner->GetComponent<TransformComponent>();
            if (tr) tr->SetPosition(current);
        }
    };

    class OKFlashEffect : public EffectComponent {
    private:
        float base_intensity = 1.0f;
        float flash_intensity = 1.5f;
        SDL_Color original_color{};
        bool initialized = false;

        float frequency = 12.0f;
        float decay_rate = 2.5f;
        float pulse_speed = 8.0f;

    public:
        void Init() override {
            auto spr = owner->GetComponent<SpriteComponent>();
            if (spr) {
                original_color = spr->GetColorMod();
                base_intensity = 1.0f;
                initialized = true;
            }
        }

        void OnFinished() override {
            auto spr = owner->GetComponent<SpriteComponent>();
            if (spr && initialized) {
                spr->SetColorMod(original_color);
            }
        }

        void Configure(float intensity) {
            flash_intensity = intensity; 
        }

        void Apply(float t, float dt, Context&) override {
            if (!initialized) return;

            auto spr = owner->GetComponent<SpriteComponent>();
            if (!spr) return;

            float decay = 1.0f - powf(t, decay_rate);
            float phase1 = t * frequency * 6.283185f;
            float phase2 = t * frequency * 3.14159f;
            float flash_wave = 0.5f + 0.3f * sinf(phase1) + 0.2f * sinf(phase2);
            float pulse = 0.5f + 0.5f * sinf(t * pulse_speed * 6.283185f);
            float intensity = decay * flash_intensity * flash_wave * pulse;
            SDL_Color flash_color = {
                static_cast<Uint8>(original_color.r * intensity),
                static_cast<Uint8>(original_color.g * intensity),
                static_cast<Uint8>(original_color.b * intensity),
                original_color.a
            };

            spr->SetColorMod(flash_color);
        }
    };

    class SceneFadeEffect : public EffectComponent {
    private:
        bool fade_in = true;
        float duration_backup = 0.f;
        SDL_Color color{ 0, 0, 0, 255 };

    public:
        void Configure(bool in, float dur = 0.8f, const SDL_Color& fade_color = { 0,0,0,255 }) {
            fade_in = in;
            duration = dur;
            duration_backup = dur;
            color = fade_color;
        }

        void Init() override { }

        void Apply(float t, float, Context& ctx) override {
            Uint8 alpha;
            float eased = 1.0f - powf(1.0f - t, 3.0f);
            alpha = static_cast<Uint8>(fade_in ? 255 * (1.0f - eased) : 255 * eased);

            SDL_BlendMode old_blend;
            auto render = ctx.GetRenderer().GetSDLRenderer();
            SDL_GetRenderDrawBlendMode(render, &old_blend);

            SDL_SetRenderDrawBlendMode(render, SDL_BLENDMODE_BLEND);
            color.a = alpha;
            SDL_SetRenderDrawColor(render,
                color.r, color.g, color.b, color.a);

            SDL_RenderFillRect(render, nullptr);

            SDL_SetRenderDrawBlendMode(render, old_blend);
        }

        void OnFinished() override { }
    };

    class SwingEffect : public EffectComponent {
        float amplitude = 30.0f;
        float angular_speed = 1.0f;
        float original_rot = 0.0f;
        float total_time = 0.0f;

    public:
        void Init() override {
            auto tr = owner->GetComponent<TransformComponent>();
            if (tr) {
                original_rot = tr->GetRotation();
            }
            total_time = 0.0f;
        }

        void OnFinished() override {

        }

        void Configure(float amp, float speed_hz = 0.5f) {
            amplitude = amp;
            angular_speed = speed_hz * 2.0f * _PI;
        }

        void Apply(float t, float dt, Context&) override {
            total_time += dt;
            float angle = amplitude * std::sin(angular_speed * total_time);

            auto tr = owner->GetComponent<TransformComponent>();
            if (tr) {
                tr->SetRotationDeg(original_rot + angle);
            }
        }
    };

    class InfiniteBreatheColorEffect : public EffectComponent {
    private:
        SDL_Color target_color{ 255, 100, 100, 255 };
        SDL_Color base_color{};
        float frequency = 1.0f;
        float total_time = 0.0f;
        bool initialized = false;
        bool preserve_alpha = true;

    public:
        void Init() override {
            auto spr = owner->GetComponent<SpriteComponent>();
            if (spr) {
                base_color = spr->GetColorMod();
                if (preserve_alpha) {
                    target_color.a = base_color.a;
                }
            }
            total_time = 0.0f;
            initialized = true;
        }

        void Configure(const SDL_Color& target, float freq = 1.0f, bool keep_alpha = true) {
            target_color = target;
            frequency = freq;
            preserve_alpha = keep_alpha;
        }

        void Apply(float t, float dt, Context&) override {
            if (!initialized) return;
            total_time += dt;

            float theta = total_time * _PI * frequency;
            float wave = std::sin(theta);
            wave = std::max(0.0f, wave);

            auto spr = owner->GetComponent<SpriteComponent>();
            if (!spr) return;

            SDL_Color current{
                static_cast<Uint8>(base_color.r + (target_color.r - base_color.r) * wave),
                static_cast<Uint8>(base_color.g + (target_color.g - base_color.g) * wave),
                static_cast<Uint8>(base_color.b + (target_color.b - base_color.b) * wave),
                preserve_alpha ? base_color.a : static_cast<Uint8>(target_color.a)
            };

            spr->SetColorMod(current);
        }
    };
}