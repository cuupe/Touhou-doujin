#include "Player.h"

namespace Game::GameObject {
	Player::Player(const std::string& _name)
	{
		role = std::make_unique<Engine::Core::GameObject>("", "player");


		check = std::make_unique<Engine::Core::GameObject>("", "player_check");
	}
}

namespace Game::GameObject::PlayerUtils {
	void Init_Player(const std::string& _name, float _speed)
	{

	}


}

