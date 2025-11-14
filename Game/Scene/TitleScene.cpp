#include "TitleScene.h"
#include "Stage.h"
#include "../../Engine/UIManager.h"
#include "UIPanel/MainPanel.h"
//#include "../UI/SettingsPanel.h"
//#include "../UI/InformationPanel.h"

#include "../../Engine/EffectManager.h"
#include "../../Engine/Components/TransformComponent.h"
#include "../../Engine/Components/SpriteComponent.h"

namespace Game::Scene {
    TitleScene::TitleScene(const std::string& name, Engine::Core::Context& ctx,
        Engine::Scene::SceneManager& sm, Engine::Audio::AudioManager& _au, Engine::UISystem::UIManager& _ui)
        : Engine::Scene::Scene(name, ctx, sm), au(_au), ui(_ui)
    { }

    void TitleScene::CreateBackgrounds()
    {
        using namespace Engine::Core::Components;
        std::pair<std::string, std::pair<Engine::Maths::Vec2, SDL_FRect>> c{
            "title_bk00", {{0.0f,0.0f}, {0.0f,0.0f,1280.0f,960.0f}} };
        std::unique_ptr<Engine::Core::GameObject> background = std::make_unique<Engine::Core::GameObject>();;
        background->AddComponent<TransformComponent>(c.second.first);
        background->AddComponent<SpriteComponent>(c.first, ctx.GetResourceManager(), c.second.second);
        AddGameObject(std::move(background));
    }

    void TitleScene::Init()
    {
        CreateBackgrounds();
        ui.RequestPushPanel(std::make_unique<Game::UI::MainMenuPanel>(au, scene_manager));
        Scene::Init();
    }

    void TitleScene::HandleInput()
    {
        Scene::HandleInput();
        ui.HandleInput();
    }

    void TitleScene::Update(float dt)
    {
        Scene::Update(dt);
        ui.Update(dt);
    }

    void TitleScene::Render()
    {
        Scene::Render();
        ui.Render();
    }
}