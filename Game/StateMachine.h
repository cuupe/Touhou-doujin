#pragma once
namespace Game::State {
	enum class PlayerAction {
		START,
		IDEL,	//IDEL与MOVE_FORWARD类型相同
		MOVE_BACK,
		MOVE_LEFT,
		MOVE_RIGHT,
		SKILL,
		SLOW,
		SLOW_SKILL
	};
}