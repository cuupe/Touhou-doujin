#pragma once
#include "../GameData.h"
#include "../../Engine/GameObject.h"
#include "../../Engine/maths.h"
#include "../../Engine/animation.h"
#include "../../Engine/sprite.h"
#include "../../Engine/collider.h"
#include "../../Engine/context.h"
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

	struct BulletData {
		std::string sprite_name;
		SDL_FRect rect;
		Engine::Maths::Vec2 position;
		Engine::Maths::Vec2 graze_size;
		Engine::Maths::Vec2 hit_size;
		Engine::Maths::Vec2 v;
		f32 angle_acc;
		f32 angle;
		f32 speed_acc;
		f32 speed;
		bool is_grazed = false;
		bool is_active = false;
	};

	class Bullet final{
	private:
		std::unique_ptr<Engine::Core::GameObject> core;
		std::unique_ptr<Engine::Core::GameObject> graze;
		BulletData bd;
		Engine::Core::Context& ctx;
	public:
		Bullet(BulletData& bullet_data,
			Engine::Core::Context& _ctx);

	public:
		Engine::Core::GameObject* GetGameObject() const { return core.get(); }
		Engine::Core::GameObject* GetGrazeObject() const { return graze.get(); }
		BulletData& GetBulletData() { return bd; }

	public:
		void Update(f32);
		void Render();
	};
}