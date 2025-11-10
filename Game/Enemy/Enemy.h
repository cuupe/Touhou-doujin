#pragma once
#include "../GameData.h"
#include "../../Engine/maths.h"
#include "../../Engine/GameObject.h"
#include "../../Engine/sprite.h"
#include "../../Engine/animation.h"
#include "../Bullet/Bullet.h"

namespace Game::GameObject {
	class Enemy {
	private:
		std::unique_ptr<Engine::Core::GameObject> body;
		



	public:
		Enemy(const std::string&);
		~Enemy() = default;
	};

	namespace EnemyUtils {
		void SpawnEnemy();

	}
}
