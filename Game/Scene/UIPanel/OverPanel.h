#pragma once
#include "../../../Engine/ui.h"
#include "../../GameData.h"
#include <vector>
#include <string>
#include <memory>

// 前置声明
namespace Engine::Audio { class AudioManager; }
namespace Engine::Scene { class SceneManager; }
namespace Engine::UISystem { class UIManager; }
namespace Engine::Core { class GameObject; class Context; }

namespace Game::UI {
    class OverPanel final : public Engine::UISystem::Panel {
    private:
        Engine::UISystem::UIManager& ui;

        enum class State {
            NAME_INPUT,
            MENU
        } current_state = State::NAME_INPUT;
        static const int MAX_RANKINGS = 10;
        std::vector<ScoreEntry> rankings;
        int current_rank_index = -1;

        std::string current_input_name;
        const size_t max_name_length = 8;

        int cursor_row = 0;
        int cursor_col = 0;

        float blink_timer = 0.0f;
        bool show_cursor = true;

        std::vector<std::vector<std::unique_ptr<Engine::Core::GameObject>>> opts;
        std::vector<std::vector<std::unique_ptr<Engine::Core::GameObject>>> bcgs;

        enum class OptType {
            NONE,
            LEAVE,
            RESTART
        } opt_type = OptType::NONE;

        int16_t menu_index = 0;
        int16_t menu_old_index = -1;
        int16_t menu_layer = 1;
        bool is_menu_init = false;

    public:
        OverPanel(const std::string& ui_name, Engine::UISystem::UIManager& _ui);

        void Init(Engine::Core::Context& ctx) override;
        void HandleInput(Engine::Core::Context& ctx, Engine::Audio::AudioManager& au,
            Engine::Scene::SceneManager& sm) override;
        void Update(f32 d_t, Engine::Core::Context& ctx) override;
        void Render(Engine::Core::Context& ctx) override;

        void OnEnter() override;
        void OnActivate() override {}
        void OnLeave() override {}

    private:
        void LoadScores();
        void SaveScores();
        std::string GetCurrentDate();
        void FinishInput(Engine::Core::Context& ctx); // 完成输入，切换到菜单

        // 菜单功能
        void InitMenuComponents(Engine::Core::Context& ctx);
        void HandleMenuInput(Engine::Core::Context& ctx, Engine::Audio::AudioManager& au, Engine::Scene::SceneManager& sm);
        void OptChooseLayer1(Engine::Core::Context& ctx, Engine::Audio::AudioManager& au, Engine::Scene::SceneManager& sm);
        void OptChooseLayer2(Engine::Core::Context& ctx, Engine::Audio::AudioManager& au, Engine::Scene::SceneManager& sm);
    };
}