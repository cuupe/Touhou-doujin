#pragma once
#include "../GameData.h"
#include "../../Engine/GameObject.h"
#include "../../Engine/maths.h"
#include "../../Engine/animation.h"
#include "../../Engine/sprite.h"
#include "../../Engine/collider.h"
#include "../../Engine/context.h"
namespace Game::Bullets {
	struct BulletData {
		std::string sprite_name;
		SDL_FRect rect;
		Engine::Maths::Vec2 position;
		Engine::Maths::Vec2 graze_size;
		Engine::Maths::Vec2 hit_size;
		Engine::Maths::Vec2 v;
		f32 angle_acc;
		f32 angle;
		f32 max_angle;
		bool restrict_angle = false;
		f32 speed_acc;
		f32 speed;
		f32 max_speed;
		bool restrict_speed = false;
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