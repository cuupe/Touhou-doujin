#include "prefix.h"
#include "Game/Game.h"
int main()
{
	SetConsoleOutputCP(CP_UTF8);

	Game g("game", 1280, 960, 0, 60);
	if (g.GetInit()) {
		g.Run();
	}
	else {
		spdlog::error("未能初始化游戏");
	}

	return 0;
}





