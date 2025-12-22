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
namespace Game::Scene {
    class Stage;
}
namespace Game::UI {
    class PausePanel final : public Engine::UISystem::Panel {
    private:
        std::vector<std::vector<std::unique_ptr<Engine::Core::GameObject>>> opts;
        std::vector<std::vector<std::unique_ptr<Engine::Core::GameObject>>> bcgs;
        Engine::UISystem::UIManager& ui;
        enum class OptType {
            NONE,
            LEAVE,
            RESTART
        }type = OptType::NONE;
        i16 index = 0;
        i16 old_index = -1;
        i16 layer = 1;
        bool first = true;
        bool is_init = false;

    public:
        PausePanel(const std::string& ui_name,Engine::UISystem::UIManager& _ui, Game::Scene::Stage* st) :
            Engine::UISystem::Panel(ui_name), ui(_ui) { }

    public:
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
        void OptChooseLayer1(Engine::Core::Context& ctx, Engine::Audio::AudioManager& au,
            Engine::Scene::SceneManager& sm);
        void OptChooseLayer2(Engine::Core::Context& ctx, Engine::Audio::AudioManager& au,
            Engine::Scene::SceneManager& sm);
    };
}