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
        float timer = 0.0f;
        float duration = 0.0f;
        bool finished = false;

    public:
        void Start(float dur) { duration = dur; timer = 0.0f; finished = false; }
        [[nodiscard]] bool IsFinished() const noexcept { return finished; }

    protected:
        virtual void Apply(float t, float dt, Context& ctx) = 0;
        virtual void OnFinished() {};

        void Update(float dt, Engine::Core::Context& ctx) {
            if (finished) {
                return;
            }
                
            timer += dt;

            float t = (duration > 0.0f) ? (timer / duration) : timer;

            if (duration > 0.0f && timer >= duration) {
                t = duration;
                finished = true;
                OnFinished();
                return;
            }

            Apply(t, dt, ctx);
        }
    };

}