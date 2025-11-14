#include "UIManager.h"
#include "ui.h"
#include "context.h"
#include <spdlog/spdlog.h>

namespace Engine::UISystem {
    UIManager::UIManager(Core::Context& _ctx, Audio::AudioManager& _au, Scene::SceneManager& _sc)
        :ctx(_ctx), au(_au), sc(_sc)
    { }

    void UIManager::RequestPushPanel(std::unique_ptr<Panel>&& panel) {
        pend_act = PendingAction::PUSH;
        pend_panel = std::move(panel);
    }

    void UIManager::RequestPopPanel() {
        pend_act = PendingAction::POP;
    }

    void UIManager::RequestReplacePanel(std::unique_ptr<Panel>&& panel) {
        pend_act = PendingAction::REPLACE;
        pend_panel = std::move(panel);
    }

    void UIManager::Update(float dt) {
        if (auto panel = GetCurrentPanel()) {
            panel->Update(dt, ctx);
        }

        ProcessPendingActions();
    }

    void UIManager::HandleInput() {
        if (auto panel = GetCurrentPanel()) {
            panel->HandleInput(ctx, au, sc);
        }
    }

    void UIManager::Render() {
        for (auto& panel : panel_stack) {
            panel->Render(ctx);
        }
    }

    void UIManager::Clear() {
        while (!panel_stack.empty()) {
            panel_stack.pop_back();
        }
    }

    void UIManager::ProcessPendingActions() {
        if (pend_act == PendingAction::NONE) {
            return;
        }

        switch (pend_act) {
        case PendingAction::PUSH: {
            auto panel = std::move(pend_panel);
            PushPanel(std::move(panel));
            break;
        }
        case PendingAction::POP:
            PopPanel();
            break;
        case PendingAction::REPLACE: {
            auto panel = std::move(pend_panel);
            ReplacePanel(std::move(panel));
            break;
        }
        default: break;
        }

        pend_act = PendingAction::NONE;
    }

    void UIManager::PushPanel(std::unique_ptr<Panel>&& panel) {
        if (!panel) {
            spdlog::warn("尝试将空 Panel 压入栈");
            return;
        }

        panel->Init(ctx);
        panel->OnEnter();
        panel_stack.push_back(std::move(panel));
    }

    void UIManager::PopPanel() {
        if (panel_stack.empty()) {
            spdlog::warn("空 Panel 栈，无法弹出");
            return;
        }
        panel_stack.pop_back();
    }

    void UIManager::ReplacePanel(std::unique_ptr<Panel>&& panel) {
        if (!panel) {
            spdlog::warn("尝试用空 Panel 替换");
            return;
        }

        while (!panel_stack.empty()) {
            panel_stack.pop_back();
        }

        panel->Init(ctx);
        panel->OnEnter();
        panel_stack.push_back(std::move(panel));
    }

}
