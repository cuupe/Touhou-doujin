#include "GameScene.h"

namespace Game::Scene {
    GameScene::GameScene(const std::string& name, Engine::Core::Context& ctx, Engine::Scene::SceneManager& s_m)
        :Scene(name, ctx, s_m)
    {
        spdlog::trace("GameScene 创建完成");
    }

    void GameScene::Init()
    {
        test();

        Scene::Init();
    }

    void GameScene::Update(float d_t) {
        Scene::Update(d_t);
    }

    void GameScene::Render() {
        Scene::Render();
    }

    void GameScene::HandleInput() {
        Scene::HandleInput();
    }

    void GameScene::Destroy() {
        Scene::Destroy();
    }

    void GameScene::test() {
        auto obj = std::make_unique<Engine::Core::GameObject>("obj");
        obj->AddComponent<Engine::Core::Components::TransformComponent>(
            Engine::Maths::Vec2(0, 0)
        );
        obj->AddComponent<Engine::Core::Components::SpriteComponent>(
            "rank00", ctx.GetResourceMannager()
        );

          AddGameObject(std::move(obj));
    }

}