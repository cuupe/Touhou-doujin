#pragma once
#include "../../../Engine/ui.h"
#include "../../../Engine/maths.h"
#include "../../GameData.h"
#include <vector>
#include <unordered_map>
namespace Engine::Audio {
    class AudioManager;
}
namespace Engine::Scene {
    class SceneManager;
}
namespace Engine::UISystem {
    class UIManager;
}
namespace Engine::Render {
    class TextManager;
}
namespace Game::UI {
	class ScorePanel : public Engine::UISystem::Panel {
    private:
        std::vector<std::unique_ptr<Engine::Core::GameObject>> data_show;
        Engine::Render::TextManager& tm;
        struct LifeConfig {
            float offset;
            Engine::Maths::Vec2 base_position;
            Engine::Maths::Vec2 scale;
            std::vector<SDL_FRect> rects;
        }life_config;

    public:
        ScorePanel(const std::string& ui_name, Engine::Render::TextManager& _tm)
            :Engine::UISystem::Panel(ui_name), tm(_tm){ }
        void Init(Engine::Core::Context& ctx) override;
        void HandleInput(Engine::Core::Context& ctx, Engine::Audio::AudioManager& au,
            Engine::Scene::SceneManager& sm) override { };
        void Update(f32, Engine::Core::Context&) override;
        void Render(Engine::Core::Context&) override;

    public:
        void OnEnter() override;
        void OnActivate() override;
        void OnLeave() override;

    private:
        void RenderData(Engine::Core::Context& ctx);
	};
}