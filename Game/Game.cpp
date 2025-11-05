#include "Game.h"
#include "../Engine/Components/SpriteComponent.h"
#include "../Engine/collider.h"
#include "../Engine/Components/ColliderComponent.h"
#include "GameScene.h"

namespace Game {
    //test用

    bool rec = false;
    static SDL_FRect square = { 350, 250, 100, 100 };
    static Engine::Maths::Vec2 pos{ 30.f, 30.f };
    static float offset = 0;
    static float line = 0;


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
        }
        catch (const std::exception& e) {
            spdlog::error("初始化失败", e.what());
            return;
        }

        //test load
        {
            bgm = TrackPtr(MIX_CreateTrack(mixer));
            if (!bgm.get()) {
                return;
            }
            res->LoadTexture(renderer, "resources/textures/player/pl00/pl00.png");
            res->LoadTexture(renderer, "resources/textures/UI/rank00.png");
            res->LoadTexture(renderer, "resources/textures/enemy/enemy5.png");
            res->LoadAudio(mixer, "resources/audios/bgm/menu.wav");
            MIX_SetTrackAudio(bgm.get(), res->GetAudio("menu")->audio.get());
            bool ok = MIX_PlayTrack(bgm.get(), 0);
            if (!ok) {
                spdlog::error("无法播放音频{}", SDL_GetError());
                return;
            }
        }
        //test -- gameobject
        {
            auto scene = std::make_unique<Scene::GameScene>("pao", *ctx, *sc);

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
            if (keystate[SDL_SCANCODE_UP]) {

                pos.y -= 5;
                if (pos.y < 20) {
                    pos.y = 20;
                }
            }
            if (keystate[SDL_SCANCODE_DOWN]) {

                pos.y += 5;
                if (pos.y > 700) {
                    pos.y = 700;
                }
            }
            if (keystate[SDL_SCANCODE_LEFT]) {
                pos.x -= 5;
                if (pos.x < 0) {
                    pos.x = 0;
                }
            }
            if (keystate[SDL_SCANCODE_RIGHT]) {
                pos.x += 5;
                if (pos.x > 700) {
                    pos.x = 700;
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
        //test
        {
            //if (!gb.IsNeedRemove()) {
            //    gb.GetComponent<Engine::Core::Components::TransformComponent>()->SetPosition(pos);
            //    gb.GetComponent<Engine::Core::Components::SpriteComponent>()->SetRect(SDL_FRect{ offset, line, 256.0 / 8, 48 });





        }
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



