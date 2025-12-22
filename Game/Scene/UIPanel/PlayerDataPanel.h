#pragma once
#include "../../../Engine/ui.h"
#include "../../GameData.h"
#include <vector>
#include <string>
#include <memory>

namespace Engine::Audio { class AudioManager; }
namespace Engine::Scene { class SceneManager; }
namespace Engine::UISystem { class UIManager; }
namespace Engine::Core { class GameObject; class Context; }

namespace Game::UI {
    class PlayerDataPanel final : public Engine::UISystem::Panel {
    private:
        Engine::UISystem::UIManager& ui;
        std::vector<std::unique_ptr<Engine::Core::GameObject>> opts;
        std::vector<std::unique_ptr<Engine::Core::GameObject>> info_bcgs;
        std::vector<ScoreEntry> rankings;
        static const int MAX_RANKINGS = 10;
        bool first = true;
        bool is_init = false;
        f32 alpha = 0.0f;
        struct Information {
            struct TextConfig {
                f32 font_size;
                SDL_Color color;
                f32 line_spacing;
            }title_config, content_config;
            std::string title;
        } info;

    public:
        PlayerDataPanel(const std::string& ui_name, Engine::UISystem::UIManager& _ui)
            :Engine::UISystem::Panel(ui_name), ui(_ui) {}

    public:
        void Init(Engine::Core::Context& ctx) override;
        void HandleInput(Engine::Core::Context& ctx, Engine::Audio::AudioManager& au,
            Engine::Scene::SceneManager& sm) override;
        void Update(f32 d_t, Engine::Core::Context& ctx) override;
        void Render(Engine::Core::Context& ctx) override;

    public:
        void OnEnter() override;
        void OnActivate() override;
        void OnLeave() override;

    private:
        void LoadScores();
        void UpdateAnimations(f32 d_t);
    };
}