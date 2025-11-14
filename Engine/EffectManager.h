#pragma once
#include "effects.h"
#include <memory>

namespace Engine::Render::Effect {
    class EffectManager {
    public:
        // 抖动
        static void Shake(Engine::Core::GameObject* go, float duration = 0.16f, float amp = 6.f) {
            auto* e = go->AddComponent<ShakeEffect>();
            e->Configure(amp);
            e->Start(duration);
        }

        // 红闪
        static void Flash(Engine::Core::GameObject* go, float duration = 0.3f) {
            auto* e = go->AddComponent<FlashEffect>();
            e->Start(duration);
        }

        // 脉冲
        static void Pulse(Engine::Core::GameObject* go, float duration = 0.8f,
            float min_s = 0.9f, float max_s = 1.1f) {
            auto* e = go->AddComponent<PulseEffect>();
            e->Configure(min_s, max_s);
            e->Start(duration);
        }

        // 渐隐
        static void FadeOut(Engine::Core::GameObject* go, float duration = 0.3f) {
            auto* e = go->AddComponent<FadeEffect>();
            e->Configure(false);
            e->Start(duration);
        }

        // 渐显
        static void FadeIn(Engine::Core::GameObject* go, float duration = 0.3f) {
            auto* e = go->AddComponent<FadeEffect>();
            e->Configure(true);
            e->Start(duration);
        }
    };

}