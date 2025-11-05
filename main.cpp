#include "prefix.h"
#include "Game/Game.h"
int main()
{
#ifdef _DEBUG
	SetConsoleOutputCP(CP_UTF8);
	spdlog::set_level(spdlog::level::debug);
#endif


	Game::Game g("测试1", 1280, 960, 0, 60);
	if (g.GetInit()) {
		g.Run();
	}
	else {
		spdlog::error("未能初始化游戏");
	}

	return 0;
}





