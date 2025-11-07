#pragma once
#include "../GameData.h"
#include "../../Engine/maths.h"
#include "../../Engine/GameObject.h"
#include "../../Engine/sprite.h"
#include "../../Engine/animation.h"
#include "../Bullet/Bullet.h"
namespace Game::GameObject {
	struct EnemyBulletShooter {
		Game::Bullets::BulletTypeSprites sprites;
		Engine::Maths::Vec2 position;
		u16 sprite;
		i16 sprite_offset;
		f32 angle1;
		f32 angle2;
		f32 speed1;
		f32 speed2;
		f32 exFloats[4];		//0 -> 角速度， 1-> 速度
		i32 exInts[4];
		i16 count1;
		i16 count2;
		u16 aim_mode;
		u32 flags;
		SoundIndex sfx;
	};

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
