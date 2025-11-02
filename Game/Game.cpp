#include "Game.h"
#include "../Engine/Components/SpriteComponent.h"
#include "GameScene.h"

namespace Game {
    //test用
    Engine::Core::GameObject gb("instance", "player");
    bool rec = false;
    static SDL_FRect square = { 350, 250, 100, 100 };
    static Engine::Maths::Vec2 pos{ 0.f,0.f };
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
            ctx = std::make_unique<Engine::Core::Context>(*r, *res);
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
            res->LoadAudio(mixer, "resources/audios/bgm/menu.wav");
            res->LoadTexture(renderer, "resources/textures/UI/rank00.png");
            MIX_SetTrackAudio(bgm.get(), res->GetAudio("menu")->audio.get());
            bool ok = MIX_PlayTrack(bgm.get(), 0);
            if (!ok) {
                spdlog::error("无法播放音频{}", SDL_GetError());
                return;
            }
        }
        //test -- gameobject
        {
            gb.AddComponent<Engine::Core::Components::TransformComponent>(
                Vec2{ 0, 20 }, Vec2{ 2, 2 });

            gb.AddComponent<Engine::Core::Components::SpriteComponent>(
                "pl00", *res, SDL_FRect{ offset, line, 256.0 / 8, 48 });

            

            auto scene = std::make_unique<Scene::GameScene>("pao", *ctx, *sc);

            sc->RequestPushScene(std::move(scene));
        }



        initialized = true;
    }


    Game::~Game()
    { }

    inline void Game::HandleInput()
    {
        const bool* keystate = SDL_GetKeyboardState(NULL);
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_EVENT_QUIT:
                running = false;
                break;
            case SDL_EVENT_WINDOW_MINIMIZED:
                spdlog::info("窗口最小化");
                break;
            case SDL_EVENT_WINDOW_RESTORED:  // 恢复时重绘
                spdlog::info("窗口恢复");
                break;
            case SDL_EVENT_KEY_DOWN:
            case SDL_EVENT_KEY_UP: {
                //TODO:处理按键响应

                SDL_Scancode scancode = event.key.scancode;
                bool is_down = event.key.down;
                bool is_repeat = event.key.repeat;

                break;
            }
            default:
                break;
            }
        }

        //TODO:处理轮询
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

    inline void Game::Render()
    {
        r->SetDrawColor(0x00, 0x00, 0x00, 0x0F);
        r->ClearScreen();
        r->SetDrawColor(0xFF, 0xFF, 0xFF, 0xFF);

        //test
        {
            

            sc->Render();
            gb.Render(*ctx);

            long long se = t.GetCount();
            if (se > 0 && se % 10 == 0) {
                offset += (!rec ? 256.0 / 8 : -256.0 / 8);
                if (offset >= 256 - 256 / 8) {
                    rec = true;
                }
                else if (offset <= 0) {
                    rec = false;
                }

            }
        }

        r->Present();
    }

    inline void Game::Update()
    {
        t.Update();

        sc->Update(t.DeltaTime());
        
        gb.GetComponent<Engine::Core::Components::TransformComponent>()->SetPosition(pos);
        gb.GetComponent<Engine::Core::Components::SpriteComponent>()->SetRect(SDL_FRect{ offset, line, 256.0 / 8, 48 });
    }


    void Game::Run()
    {
        while (running) {
            HandleInput();
            Render();
            Update();
        }
    }


    void Game::test()
    {


    }
}



