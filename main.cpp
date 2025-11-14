#include "prefix.h"
#include "Game/Game.h"
int main()
{
#ifdef _DEBUG
	SetConsoleOutputCP(CP_UTF8);
#endif

	Game::MGame g("测试3", 1280, 960, WINDOWED, 60);
	if (g.GetInit()) {
		g.Run();
	}
	else {
		spdlog::error("未能初始化游戏");
	}

	return 0;
}





