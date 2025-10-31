#include "Game.h"

//test用
static SDL_FRect square = { 350, 250, 100, 100 };
using namespace Resources;
Game::Game(const char* win_name, int width, int height, int flag, int fps)
	:Engine::engine(win_name, width, height, flag, fps)
    //TODO:DIRECTX初始化
{ 
    //Test样例
    bgm = TrackPtr(MIX_CreateTrack(mixer));
    if (!bgm.get()) {
        return;
    }
    texs.insert({"rank00",
        std::move(LoadTexture(renderer, "resources/textures/UI/rank00.png"))});
    audios.insert({ "menu", LoadAudio(mixer, "resources/audios/bgm/menu.wav") });
    MIX_SetTrackAudio(bgm.get(), audios["menu"]->audio.get());
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
    }
    if (keystate[SDL_SCANCODE_DOWN]) {
        square.y += 2.5;
    }
    if (keystate[SDL_SCANCODE_LEFT]) {
        square.x -= 2.5;
    }
    if (keystate[SDL_SCANCODE_RIGHT]) {
        square.x += 2.5;
    }
}

inline void Game::Render()
{
    // 清屏
    SDL_SetRenderDrawColor(GetRenderer(), 0x20, 0x30, 0x30, 0xFF);
    SDL_RenderClear(GetRenderer());

    SDL_SetRenderDrawColor(GetRenderer(), 0xFF, 0xFF, 0xFF, 0xFF);
    
    //todo:
    SDL_FRect dstRect = {
            0.f,
            0.f,
            static_cast<float>(texs["rank00"]->width),
            static_cast<float>(texs["rank00"]->height)
    };

    SDL_FRect* srcRect = NULL;

    SDL_RenderTexture(
        renderer,
        texs["rank00"]->texture.get(), 
        srcRect, 
        &dstRect   
    );

    
    SDL_RenderFillRect(GetRenderer(), &square);

    // 渲染
    SDL_RenderPresent(GetRenderer());
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



