#pragma once
#include "effects.h"
#include <memory>

namespace Engine::Render::Effect::EffectManager {
    static void Shake(Engine::Core::GameObject* go, float duration = 0.16f, float amp = 6.0f) {
        auto e = go->AddComponent<ShakeEffect>();
        e->Configure(amp);
        e->Start(duration);
    }

    static void Flash(Engine::Core::GameObject* go, float duration = 0.3f) {
        auto e = go->AddComponent<FlashEffect>();
        e->Start(duration);
    }

    static void Pulse(Engine::Core::GameObject* go, float duration = 0.8f,
        float min_s = 0.9f, float max_s = 1.1f) {
        auto e = go->AddComponent<PulseEffect>();
        e->Configure(min_s, max_s);
        e->Start(duration);
    }

    static void FadeOut(Engine::Core::GameObject* go, float duration = 0.3f) {
        auto* e = go->AddComponent<FadeEffect>();
        e->Configure(false);
        e->Start(duration);
    }

    static void FadeIn(Engine::Core::GameObject* go, float duration = 0.3f) {
        auto e = go->AddComponent<FadeEffect>();
        e->Configure(true);
        e->Start(duration);
    }


    static void Blink(Engine::Core::GameObject* go, float freq = 1.0f, 
        float min_alpha = 150 , float duration = 0.0f) {
        auto e = go->AddComponent<BlinkEffect>();
        e->Configure(freq, min_alpha);
        e->Start(duration);
    }

    static void StopBlink(Engine::Core::GameObject* go) {
        go->RemoveComponent<BlinkEffect>();
        auto spr = go->GetComponent<Engine::Core::Components::SpriteComponent>();
        if (spr) {
            spr->SetColorMod({ 255, 255, 255, 255 });
            spr->SetScale({ 1.0f, 1.0f });
        }
    }

    static void SlideIn(Engine::Core::GameObject* go,
        float duration = 0.5f, Vec2 from_offset = { -200.0f, 0.0f }) {
        auto e = go->AddComponent<SlideEffect>();
        e->Configure(from_offset);
        e->Start(duration);
    }

    static void RotateIn(Engine::Core::GameObject* go, 
        float duration = 0.5f, float from_angle = 90.0f) {
        auto e = go->AddComponent<RotateEffect>();
        e->Configure(from_angle);
        e->Start(duration);
    }

    static void BounceIn(Engine::Core::GameObject* go, 
        float duration = 0.6f, float amp = 1.2f) {
        auto e = go->AddComponent<BounceEffect>();
        e->Configure(amp);
        e->Start(duration);
    }

    static void ScaleIn(Engine::Core::GameObject* go, 
        float duration = 0.4f, float from_scale = 0.0f) {
        auto e = go->AddComponent<ScaleEffect>();
        e->Configure(from_scale, 1.0f);
        e->Start(duration);
    }

    static void SlideLeftOut(Engine::Core::GameObject* go, float duration = 0.4f, float screen_w = 1280.0f) {
        auto e = go->AddComponent<SlideDirectionEffect>();
        e->Configure({ -screen_w, 0.0f }, false);
        e->Start(duration);
    }

    static void SlideRightIn(Engine::Core::GameObject* go, float duration = 0.4f, float screen_w = 1280.0f) {
        auto e = go->AddComponent<SlideDirectionEffect>();
        e->Configure({ screen_w, 0.0f }, true);
        e->Start(duration);
    }

    static void OKFlash(Engine::Core::GameObject* go, float duration = 0.6f, float intensity = 1.5f) {
        auto e = go->AddComponent<OKFlashEffect>();
        e->Configure(intensity);
        e->Start(duration);
    }
}