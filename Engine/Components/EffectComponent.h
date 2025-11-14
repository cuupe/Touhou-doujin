#pragma once
#include "../component.h"
#include <memory>
namespace Engine::Core {
    class Context;
}
namespace Engine::Core::Components {
    class EffectComponent: public Component {
        friend class GameObject;
    protected:
        float timer = 0.f;
        float duration = 0.f;
        bool  finished = false;

    public:
        void Start(float dur) { duration = dur; timer = 0.f; finished = false; }
        [[nodiscard]] bool IsFinished() const noexcept { return finished; }

    protected:
        virtual void Apply(float t, float dt, Context& ctx) = 0;

        void Update(float dt, Engine::Core::Context& ctx) {
            if (finished) return;
            timer += dt;
            if (timer >= duration) {
                timer = duration;
                finished = true;
            }
            float t = duration > 0.f ? timer / duration : 0.f;
            Apply(t, dt, ctx);
        }
    };

}