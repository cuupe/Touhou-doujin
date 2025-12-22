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
    class InformationPanel final : public Engine::UISystem::Panel {
    private:
        std::vector<std::unique_ptr<Engine::Core::GameObject>> opts;
        std::vector<std::unique_ptr<Engine::Core::GameObject>> info_bcgs;
        Engine::UISystem::UIManager& ui;
        bool first = true;
        bool is_init = false;
        struct Information {
            struct TextConfig {
                f32 font_size;
                SDL_Color color;
                f32 line_spacing;
            };
            std::string title;
            TextConfig title_config;
            std::vector<std::string> content;
            TextConfig content_config;
        }info;
        f32 alpha = 0.0f;
        
    public:
        InformationPanel(const std::string& ui_name, Engine::UISystem::UIManager& _ui)
            :Engine::UISystem::Panel(ui_name), ui(_ui) { }

    public:
        void Init(Engine::Core::Context& ctx) override;
        void HandleInput(Engine::Core::Context& ctx, Engine::Audio::AudioManager& au,
            Engine::Scene::SceneManager& sm) override;
        void Update(f32, Engine::Core::Context&) override;
        void Render(Engine::Core::Context&) override;

    public:
        void PanelMainContentUpdate(f32, Engine::Core::Context&);

    public:
        void OnEnter() override;
        void OnActivate() override;
        void OnLeave() override;

    };
}