#include "Game.h"
#include "Scene/Stage.h"
#include "Scene/Loading.h"
#include "../Engine/schedule.h"
#include "../Engine/CameraManager.h"
namespace Game {
    using namespace Engine;
    MGame::MGame(const char* win_name, const char* icon_path, int width, int height, int flag, int fps)
        :Engine::engine(win_name, icon_path, width, height, flag, fps)
    {
        try {
            res = std::make_unique<Engine::Resource::ResourceManager>(device.Get());
            sch = std::make_unique<Engine::Time::Schedule>();
            cm = std::make_unique<Engine::Render::CameraManager>();
            r = std::make_unique<Engine::Render::Renderer>(renderer, res.get(), cm.get(),
                device.Get(), device_context.Get(), width, height);
            input = std::make_unique<Engine::Input::InputManager>(r.get());
            ctx = std::make_unique<Engine::Core::Context>(*r, *res, *input, *sch, *device.Get(), *device_context.Get());
            sm = std::make_unique<Engine::Scene::SceneManager>(*ctx);
            text = std::make_unique<Engine::Render::TextManager>(*ctx, "data/ascii.json");
            audio = std::make_unique<Engine::Audio::AudioManager>(*ctx);
            if (audio) {
                audio->SetBGMSound(config.bgm_sound / 100.0f);
                audio->SetSFXSound(config.sfx_sound / 100.0f);
            }
            ui = std::make_unique<Engine::UISystem::UIManager>(*ctx, *audio, *text, *sm);
        }
        catch (const std::exception& e) {
            spdlog::error("初始化失败", e.what());
            return;
        }

        auto ss = std::make_unique<Scene::Loading>("load", *ctx, *sm, *audio, *ui, *text);
        sm->RequestPushScene(std::move(ss));

        initialized = true;
    }

    MGame::~MGame()
    { }

    inline void MGame::HandleInput()
    {
        sm->HandleInput();
    }

    inline void MGame::Render()
    {
        r->SetDrawColor(0, 0, 0, 255);
        r->ClearScreen();
        sm->Render();

        r->Present();
    }

    inline void MGame::Update()
    {
        input->Update();
        t.Update();
        sch->Update(t.DeltaTime());
        sm->Update(t.DeltaTime());

        if (input->ShouldQuit()) {
            running = false;
        }
    }


    void MGame::Run()
    {
        while (running) {
            HandleInput();
            Update();
            Render();
        }
    }
}



