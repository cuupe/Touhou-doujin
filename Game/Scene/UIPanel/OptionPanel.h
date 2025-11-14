#pragma once
#include "../../../Engine/ui.h"
#include "../../GameData.h"
namespace Engine::Audio {
    class AudioManager;
}
namespace Engine::Scene {
    class SceneManager;
}
namespace Engine::UISystem {
    class UIManager;
}
namespace Game::UI {
    class OptionPanel final : public Engine::UISystem::Panel {
    private:
        std::vector<std::unique_ptr<Engine::Core::GameObject>> opts;
        std::vector<std::unique_ptr<Engine::Core::GameObject>> bcgs;
        Engine::Audio::AudioManager& au;
        Engine::Scene::SceneManager& scene_manager;
        i16 index = 0;
        i16 old_index = -1;
        bool first = true;

    public:
        OptionPanel(Engine::Audio::AudioManager& _au, Engine::Scene::SceneManager& sm)
            : au(_au), scene_manager(sm) {
        }

        void Init(Engine::Core::Context& ctx) override;
        void HandleInput(Engine::Core::Context& ctx, Engine::Audio::AudioManager& au,
            Engine::Scene::SceneManager& sm) override;
        void Update(f32, Engine::Core::Context&) override;
        void Render(Engine::Core::Context&) override;

    public:
        void OnEnter() override;
        void OnActivate() override;
        void OnLeave() override;

    public:
        void OptChoose(Engine::Core::Context& ctx);
    };

}