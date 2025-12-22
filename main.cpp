#include "prefix.h"
#include "Game/Game.h"
#include "Game/GameData.h"
#include <SDL3/SDL_main.h>
int main(int argc, char* argv[])
{
	SetConsoleOutputCP(CP_UTF8);

    SDL_MessageBoxButtonData buttons[] = {
        { 0, 0, "窗口化" },
        { 0, 1, "全屏" },
        { 0, 2 , "退出" }
    };

    SDL_MessageBoxData messageboxdata = {
        SDL_MESSAGEBOX_INFORMATION,
        nullptr,
        "请选择窗口模式：",
        "逻辑分辨率1280x960",
        SDL_arraysize(buttons),
        buttons,
        nullptr
    };

    int button_id;
    if (SDL_ShowMessageBox(&messageboxdata, &button_id) < 0) {
        spdlog::error("错误");
        return 0;
    }
    if (button_id == 2) {
        return 0;
    }


	Game::MGame g("STG", "favicon.ico", SCREEN_WIDTH, SCREEN_HEIGHT, button_id, FPS);
	if (g.GetInit()) {
		g.Run();
	}
	else {
		spdlog::error("未能初始化游戏");
	}

	return 0;
}