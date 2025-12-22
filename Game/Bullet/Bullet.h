#pragma once
#include "../GameData.h"
#include "../../Engine/GameObject.h"
#include "../../Engine/maths.h"
#include "../../Engine/animation.h"
#include "../../Engine/sprite.h"
#include "../../Engine/collider.h"
#include "../../Engine/context.h"
//旧的OOC式驱动
//namespace Game::Bullets {
//	struct BulletConfig {
//		std::string source;
//		SDL_FRect rect = {0.0f, 0.0f, 0.0f, 0.0f};
//		Engine::Maths::Vec2 hit_size;
//		std::vector<Engine::Maths::Vec2> offset;
//		std::string anim_name = "default";
//		std::vector<Engine::Render::AnimationFrame> anim;
//	};
//
//	class Bullet final{
//	private:
//		std::unique_ptr<Engine::Core::GameObject> core;
//		Engine::Core::Context& ctx;
//
//	public:
//		Bullet(BulletData&& bullet_data,
//			Engine::Core::Context& _ctx, std::unique_ptr<Engine::Render::Animation> _an);
//
//	public:
//		Engine::Core::GameObject* GetGameObject() const { return core.get(); }
//		Bullets::BulletData& GetBulletData() {
//			return core->GetComponent<Component::BulletComponent>()->GetBulletData();
//		}
//		void UpdateBulletData(const BulletConfig& bc, int offset_pitch);
//	public:
//		void Update(f32);
//		void Render();
//	};
//}

//DOD式驱动
namespace Game::Bullets {
	using f32 = float;
	using namespace Engine::Maths;

	enum BulletState {
		BS_UNUSED,
		BS_ACTIVE,
		BS_DESTROYING
	};

	struct BulletConfig {
		std::string source;
		SDL_FRect rect = { 0.0f, 0.0f, 0.0f, 0.0f };
		Vec2 hit_box;
		std::vector<Vec2> offset;
		struct BulletAnimationFrame {
			SDL_FRect source_rect;
			f32 duration;
			Engine::Maths::Vec2 scale;
			f32 rotation;
		};
		std::vector<BulletAnimationFrame> anim;
		std::vector<BulletAnimationFrame> destroy_anim_frames;
		f32 total_destroy_duration = 0.0f;
		std::string destroy_sprite_name;
	};


	struct BulletData {
		std::string sprite_name;
		SDL_FRect rect;
		Vec2 position;
		Vec2 hit_box;
		Vec2 v;
		f32 angle;
		f32 speed;
		BulletState state = BS_UNUSED;
		f32 destroy_timer = 0.0f;
		f32 angle_acc;
		f32 max_angle;
		bool restrict_angle = false;
		f32 speed_acc;
		f32 max_speed;
		bool restrict_speed = false;
		bool is_grazed = false;
		const struct BulletConfig* config_ptr = nullptr;
		i32 offset_pitch;
		f32 current_anim_timer = 0.0f;
		i32 current_anim_frame_index = 0;
	};

	constexpr i32 MAX_BULLET_SIZE = 1280;
}