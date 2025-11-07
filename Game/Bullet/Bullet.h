#pragma once
#include "../GameData.h"
#include "../../Engine/GameObject.h"
#include "../../Engine/maths.h"
#include "../../Engine/animation.h"
#include "../../Engine/sprite.h"
#include "../../Engine/collider.h"
namespace Game::Bullets {
	enum AimMode {
		FAN_AIMED,	//对称轴指向自机
		FAN,	//普通扇形
		CIRCLE_AIMED,	//初始角度指向自机
		CIRCLE,
		OFFSET_CIRCLE_AIMED,	
		OFFSET_CIRCLE,
		RANDOM_ANGLE,	// angle1 ~ angle2
		RANDOM_SPEED,	// speed1 ~ speed2
		RANDOM			// 角度速度双随机
	};

	struct BulletTypeSprites {
		Engine::Render::Animation anim;
		Engine::Render::Animation spawn_effect_fast;
		Engine::Render::Animation spawn_effect_normal;
		Engine::Render::Animation spawn_effect_slow;
		Engine::Render::Animation spawn_effect_donut;

		Engine::Maths::Vec2 graze_size;
		u8 bullet_width;
		u8 bullet_height;
	};

	struct BulletData {
		BulletTypeSprites sprites;
		Engine::Maths::Vec2 position = {0.0f, 0.0f};
		Engine::Maths::Vec2 velocity = { 0.0f, 0.0f };
		Engine::Maths::Vec2 ex4_acceleration = { 0.0f, 0.0f };
		f32 speed = 0.0f;
		f32 ex5_acceleration_speed = 0.0f;
		f32 dir_change_speed = 0.0f;
		f32 angle = 0.0f;
		f32 ex5_angular_velocity = 0.0f;
		f32 dir_change_rotation = 0.0f;
		i32 ex5_lifespan = 0;
		i32 dir_change_interval = 0;
		i32 dir_change_max_times = 0;
		i32 dir_change_num_times = 0;
		i16 sprite_offset = 0;
		u16 ex_flags = 0x0000;
		u16 state = 0;
		u16 out_stay_frames = 0; //出界计数
		u8 is_active = 0;
		u8 is_grazed = 0;
	};

	class Bullet final{
	private:
		std::unique_ptr<Engine::Core::GameObject> core;
		Game::Bullets::BulletData* bd = nullptr;
	public:
		Bullet(Game::Bullets::BulletData*);


	public:
		Game::Bullets::BulletData* GetBulletData() const { return bd; }
		Engine::Core::GameObject* GetGameObject() const { return core.get(); }
	public:
		void Update(float);
		void Render();
	};


	struct LaserData {

	};


	class Laser final{
	private:

	};

	
}