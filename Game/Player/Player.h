#pragma once
#include "../Engine/GameObject.h"
#include "../Bullet/Bullet.h"
namespace Game::GameObject {
	class Player {
	private:
		std::unique_ptr<Engine::Core::GameObject> role;
		std::unique_ptr<Engine::Core::GameObject> check;

	public:
		Player(const std::string&);
		~Player() = default;
	};



	namespace PlayerUtils {
		void Init_Player(const std::string& _name, float _speed);
	}
}

