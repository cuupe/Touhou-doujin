#include "Game.h"
#include "GameScene.h"
#include "Scene/Stage.h"
#include <thread>

namespace Game {
    //test用
    using namespace Engine;
    MGame::MGame(const char* win_name, int width, int height, int flag, int fps)
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

        //test load(后续将使用多线程)
        {

            res->LoadTexture(renderer, "resources/textures/player/pl00/pl00.png");
            res->LoadTexture(renderer, "resources/textures/UI/rank00.png");
            res->LoadTexture(renderer, "resources/textures/enemy/enemy5.png");
            res->LoadAudio(audio->GetMixer(), "resources/audios/bgm/menu.wav");
            res->LoadAudio(audio->GetMixer(), "resources/audios/sfx/se_tan03.wav");
            for (int i = 1; i <= 5; i++) {
                std::string po = "resources/textures/bullet/bullet" + std::to_string(i) + ".png";
                res->LoadTexture(renderer, po);
            }
        }

        {
            auto ss = std::make_unique<Scene::Stage>("fuck", *ctx, *sc, *audio);
            audio->PlayBGM("menu");
            sc->RequestPushScene(std::move(ss));
        }



        initialized = true;
    }

    MGame::~MGame()
    { }

    inline void MGame::HandleInput()
    {
        sc->HandleInput();
    }

    inline void MGame::Render()
    {
        r->SetDrawColor(0xFF, 0xFF, 0xFF, 0xFF);
        r->ClearScreen();

        sc->Render();

        r->Present();
    }

    inline void MGame::Update()
    {
        input->Update();
        t.Update();
        sc->Update(t.DeltaTime());
    }


    void MGame::Run()
    {
        while (running) {
            HandleInput();
            Update();
            Render();
        }
    }


    void MGame::test()
    {


    }
}



