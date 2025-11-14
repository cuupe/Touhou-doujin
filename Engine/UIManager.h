#pragma once
#include "ui.h"
#include <vector>
#include <memory>
namespace Engine::Core {
    class Context;
}
namespace Engine::Audio {
    class AudioManager;
}
namespace Engine::Scene {
    class SceneManager;
}
namespace Engine::UISystem {
    class UIManager final {
    private:
        Core::Context& ctx;
        Audio::AudioManager& au;
        Scene::SceneManager& sc;
        std::vector<std::unique_ptr<Panel>> panel_stack;

        enum class PendingAction { NONE, PUSH, POP, REPLACE };
        PendingAction pend_act = PendingAction::NONE;
        std::unique_ptr<Panel> pend_panel;

    public:
        UIManager(Core::Context&, Audio::AudioManager&,
            Scene::SceneManager&);
        ~UIManager() { Clear(); }

        UIManager(const UIManager&) = delete;
        UIManager& operator=(const UIManager&) = delete;
        UIManager(UIManager&&) = delete;
        UIManager& operator=(UIManager&&) = delete;

    public:
        Panel* GetCurrentPanel() const {
            if (panel_stack.empty()) {
                return nullptr;
            }
            return panel_stack.back().get();
        }

    public:
        void RequestPushPanel(std::unique_ptr<Panel>&& panel);
        void RequestPopPanel();
        void RequestReplacePanel(std::unique_ptr<Panel>&& panel);

        void Update(float dt);
        void Render();
        void HandleInput();
        void Clear();

    private:
        void ProcessPendingActions();
        void PushPanel(std::unique_ptr<Panel>&& panel);
        void PopPanel();
        void ReplacePanel(std::unique_ptr<Panel>&& panel);
    };

}