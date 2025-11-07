#include "GameScene.h"
#include "../Engine/collider.h"


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
        auto gb = std::make_unique<Engine::Core::GameObject>("instance");
        gb->AddComponent<Engine::Core::Components::TransformComponent>(
            Vec2{0, 0}, Vec2{2, 2});
        gb->AddComponent<Engine::Core::Components::SpriteComponent>(
            "pl00", ctx.GetResourceMannager(), SDL_FRect{ 0, 0, 256.0 / 8, 48 }, Align::CENTER);
        gb->AddComponent<Engine::Core::Components::ColliderComponent>(
            std::make_unique<Engine::Core::Collider::AABBCollider>(
                Vec2{ 10, 10 }), Align::CENTER);
        Animation anim_0("idle");
        for (float i = 0; i < 5; i += 1) {
            anim_0.AddFrame(SDL_FRect{ i * 32, 0, 32, 48 }, 0.2);
        }
        Animation anim_1("left", false);
        anim_1.AddFrame(SDL_FRect{ 32.0f, 48.0f, 32.0f, 48.0f }, 0.05f);
        anim_1.AddFrame(SDL_FRect{ 2 * 32.0f, 48.0f, 32.0f, 48.0f }, 0.05f);
        anim_1.AddFrame(SDL_FRect{ 3 * 32.0f, 48.0f, 32.0f, 48.0f }, 0.05f);
        anim_1.AddFrame(SDL_FRect{ 4 * 32.0f, 48.0f, 32.0f, 48.0f }, 0.1f);
        anim_1.AddFrame(SDL_FRect{ 5 * 32.0f, 48.0f, 32.0f, 48.0f }, 0.2f);
        
        Animation anim_2("right", false);
        anim_2.AddFrame(SDL_FRect{ 32.0f, 96.0f, 32.0f, 48.0f }, 0.05f);
        anim_2.AddFrame(SDL_FRect{ 2 * 32.0f, 96.0f, 32.0f, 48.0f }, 0.05f);
        anim_2.AddFrame(SDL_FRect{ 3 * 32.0f, 96.0f, 32.0f, 48.0f }, 0.05f);
        anim_2.AddFrame(SDL_FRect{ 4 * 32.0f, 96.0f, 32.0f, 48.0f }, 0.1f);
        anim_2.AddFrame(SDL_FRect{ 5 * 32.0f, 96.0f, 32.0f, 48.0f }, 0.2f);
        gb->AddComponent<Engine::Core::Components::AnimationComponent>();
        gb->GetComponent<Engine::Core::Components::AnimationComponent>()->AddAnimation(
            std::make_unique<Animation>(anim_0));
        gb->GetComponent<Engine::Core::Components::AnimationComponent>()->AddAnimation(
            std::make_unique<Animation>(anim_1));
        gb->GetComponent<Engine::Core::Components::AnimationComponent>()->AddAnimation(
            std::make_unique<Animation>(anim_2));
        gb->GetComponent<Engine::Core::Components::AnimationComponent>()->PlayAnimaiton("idle");
        gb->AddComponent<Engine::Core::Components::PlayerComponent>(350.0f);
        AddGameObject(std::move(gb));



        auto pic = std::make_unique<Engine::Core::GameObject>("liberty");
        pic->AddComponent<Engine::Core::Components::TransformComponent>(
            Engine::Maths::Vec2{ 300, 300 }, Engine::Maths::Vec2{ 2, 2 }
        );
        pic->AddComponent<Engine::Core::Components::SpriteComponent>(
            "enemy5", ctx.GetResourceMannager(), SDL_FRect{0, 0, 32, 32},
            Align::CENTER
        );
        pic->AddComponent<Engine::Core::Components::ColliderComponent>(
            std::make_unique<Engine::Core::Collider::AABBCollider>(
                Vec2{8, 8}), Align::CENTER
        );
        Animation anim("enemy_idle");
        for (float i = 0; i < 5; i+=1) {
            anim.AddFrame(SDL_FRect{i * 32, 0, 32, 32}, 0.2);
        }

        pic->AddComponent<Engine::Core::Components::AnimationComponent>();
        pic->GetComponent<Engine::Core::Components::AnimationComponent>()->AddAnimation(
            std::make_unique<Animation>(anim));
        pic->GetComponent<Engine::Core::Components::AnimationComponent>()->PlayAnimaiton("enemy_idle");
        AddGameObject(std::move(pic));
    }

}