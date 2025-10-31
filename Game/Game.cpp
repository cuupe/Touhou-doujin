#include "Game.h"
//test用
bool rec = false;
static SDL_FRect square = { 350, 250, 100, 100 };
static Engine::Render::Pos2 pos{ 0.f,0.f };
static float offset = 0;
static float line = 0;
using namespace Engine;
Game::Game(const char* win_name, int width, int height, int flag, int fps)
    :Engine::engine(win_name, width, height, flag, fps)
    //TODO:DIRECTX初始化
{
    res = std::make_unique<Engine::Resource::ResourceMannager>();
    r = std::make_unique<Engine::Render::Renderer>(renderer, res.get());

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
        square.y -= 2.5;
        pos.y -= 1;
    }
    if (keystate[SDL_SCANCODE_DOWN]) {
        square.y += 2.5;
        pos.y += 1;
    }
    if (keystate[SDL_SCANCODE_LEFT]) {
        square.x -= 2.5;
        pos.x -= 1;
}
    if (keystate[SDL_SCANCODE_RIGHT]) {
        square.x += 2.5;
        pos.x += 1;
    }
}

inline void Game::Render()
{
    r->SetDrawColor(0x00, 0x00, 0x00, 0x0F);
    r->ClearScreen();
    r->SetDrawColor(0xFF, 0xFF, 0xFF, 0xFF);
    //test
    {
        r->DrawUI(Engine::Render::sprite("rank00"), {0,0}, {0, 0, 1024, 1024});
        r->DrawSprite(Engine::Render::sprite("pl00"), { pos.x * 4, pos.y * 4 }, { offset, line, 256.0 / 8, 48, 2.0, 2.0 });
        long long se = t.GetCount();
        if (se > 0 && se % 15 == 0) {
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

}


void Game::Run()
{
    while (running) {
        HandleInput();
        Render();
        Update();
    }
}



