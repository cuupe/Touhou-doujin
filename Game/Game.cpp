#include "Game.h"
#include "../Engine/Components/SpriteComponent.h"
#include "../Engine/collider.h"
#include "../Engine/Components/ColliderComponent.h"
#include "GameScene.h"

namespace Game {
    //test用


    using namespace Engine;
    Game::Game(const char* win_name, int width, int height, int flag, int fps)
        :Engine::engine(win_name, width, height, flag, fps)
        //TODO:DIRECTX初始化
    {
        try {
            res = std::make_unique<Engine::Resource::ResourceMannager>();
            r = std::make_unique<Engine::Render::Renderer>(renderer, res.get());
            input = std::make_unique<Engine::Input::InputManager>(renderer);
            ctx = std::make_unique<Engine::Core::Context>(*r, *res, *input);
            sc = std::make_unique<Engine::Scene::SceneManager>(*ctx);
            audio = std::make_unique<Engine::Audio::AudioManager>(*ctx);
        }
        catch (const std::exception& e) {
            spdlog::error("初始化失败", e.what());
            return;
        }

        //test load
        {
            res->LoadTexture(renderer, "resources/textures/player/pl00/pl00.png");
            res->LoadTexture(renderer, "resources/textures/UI/rank00.png");
            res->LoadTexture(renderer, "resources/textures/enemy/enemy5.png");
            res->LoadAudio(audio->GetMixer(), "resources/audios/bgm/menu.wav");
        }
        //test -- gameobject
        {
            auto scene = std::make_unique<Scene::GameScene>("pao", *ctx, *sc);
            audio->PlayBGM("menu");
            sc->RequestPushScene(std::move(scene));
        }



        initialized = true;
    }

    Game::~Game()
    { }

    inline void Game::HandleInput()
    {
        sc->HandleInput();

        {
            const bool* keystate = SDL_GetKeyboardState(NULL);
            //TODO:处理轮询(test)
            if (keystate[SDL_SCANCODE_SPACE]) {
                if (audio->IsBGMPlaying()) {
                    audio->PauseBGM();
                }
                else {
                    audio->PlayBGM();
                }
            }
        }
    }

    inline void Game::Render()
    {
        r->SetDrawColor(0xFF, 0xFF, 0xFF, 0xFF);
        r->ClearScreen();

        sc->Render();

        r->Present();
    }

    inline void Game::Update()
    {
        input->Update();
        t.Update();
        sc->Update(t.DeltaTime());
    }


    void Game::Run()
    {
        while (running) {
            HandleInput();
            Update();
            Render();
        }
    }


    void Game::test()
    {


    }
}



