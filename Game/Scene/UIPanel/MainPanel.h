#pragma once
#include "../../../Engine/ui.h"
#include "../../GameData.h"
#include <stack>
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
    class MainPanel final : public Engine::UISystem::Panel {
    private:
        enum class State {
            MENU,
            PLAYER,
            DIFF
        };
        State current_state = State::MENU;
        std::vector<std::unique_ptr<Engine::Core::GameObject>> bcgs;
        std::vector<std::vector<std::unique_ptr<Engine::Core::GameObject>>> opts;
        Engine::UISystem::UIManager& ui;
        bool is_init = false;
        
        //MENU
        i16 index = 0;
        i16 old_index = -1;
        bool first = true;


        //PLAYER
        i16 index_player = 0;


        //DIFF
        i16 old_diff = -1;
        i16 index_diff = 0;

    public:
        MainPanel(const std::string& ui_name, Engine::UISystem::UIManager& _ui):
            Engine::UISystem::Panel(ui_name), ui(_ui){ }

        void Init(Engine::Core::Context& ctx) override;
        void HandleInput(Engine::Core::Context& ctx, Engine::Audio::AudioManager& au,
            Engine::Scene::SceneManager& sm) override;
        void Update(f32, Engine::Core::Context&) override;
        void Render(Engine::Core::Context&) override;

    public:
        void OnEnter() override;
        void OnActivate() override;
        void OnLeave() override;

    private:
        void OptChoose(Engine::Core::Context& ctx, Engine::Audio::AudioManager& au,
            Engine::Scene::SceneManager& sm);
        void MenuHandleInput(Engine::Core::Context& ctx, Engine::Audio::AudioManager& au,
            Engine::Scene::SceneManager& sm);
        void MenuUpdate(f32 dt, Engine::Core::Context& ctx);
        void MenuChoose(Engine::Core::Context& ctx, Engine::Audio::AudioManager& au,
            Engine::Scene::SceneManager& sm);
        void MenuOnEnter();
        void MenuOnLeave();
        void PlayerHandleInput(Engine::Core::Context& ctx, Engine::Audio::AudioManager& au,
            Engine::Scene::SceneManager& sm);
        void PlayerChoose(Engine::Core::Context& ctx, Engine::Audio::AudioManager& au,
            Engine::Scene::SceneManager& sm);
        void PlayerUpdate(f32 dt, Engine::Core::Context& ctx);
        void PlayerOnEnter();
        void PlayerOnLeave();
        void DiffHandleInput(Engine::Core::Context& ctx, Engine::Audio::AudioManager& au,
            Engine::Scene::SceneManager& sm);
        void DiffChoose(Engine::Core::Context& ctx, Engine::Audio::AudioManager& au,
            Engine::Scene::SceneManager& sm);
        void DiffUpdate(f32 dt, Engine::Core::Context& ctx);
        void DiffOnEnter();
        void DiffOnLeave();
        void SlideLeft();
        void SlideRight();
    };
}