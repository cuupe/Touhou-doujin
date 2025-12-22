#pragma once
#include "../Player.h"
#include <utility> 
namespace Game::Manager {
	class PlayerBulletManager;
}
namespace Game::Player {
	struct OrbState {
		Engine::Maths::Vec2 current_pos;
		Engine::Maths::Vec2 target_pos;
		float transition_timer = 0.0f;
		const float TRANSITION_DURATION = 0.1f;
		bool is_transitioning = false;
		bool last_focus_mode = false;
	};

	class Reimu : public Game::Entity::Player {
	private:
		std::unique_ptr<Engine::Core::GameObject> check_box;
		std::unique_ptr<Engine::Core::GameObject> effect;
		std::vector<std::unique_ptr<Engine::Core::GameObject>> orbs;
		std::vector<OrbState> orb_states;
		i32 current_orb_count = 1;
		f32 orbit_timer = 0.0f;
		const f32 ORB_RADIUS_NORMAL = 60.0f;
		const f32 ORB_RADIUS_FOCUSED = 50.0f;
		const f32 ORB_ROTATION_SPEED = 2.0f;
		const f32 GRAZE_RADIUS = 86.0f;
		const i32 reimu_base_fire_rate_frames = 3;
		i32 shoot_cooldown_frames = 0;
		f32 check_box_rotation = 0.0f;

		bool is_skilled = false;
	    f32 effect_size = 0.5f;
		const f32 skill_time = 5.0f;
		f32 skill_rotation = 0.0f;
		f32 skill_time_count = 0.0f;

		const std::string reimu_main_shot = "normal_shoot_1";
		const std::string reimu_curve_shot = "normal_shoot_2";
		const std::string orb_support_shot = "trait_bullet";
		bool shoot_type = false;
		const f32 main_shot_speed = 2000.0f;
		const f32 orb_support_speed = 1400.0f;

		
	public:
		Reimu(Engine::Core::Context& _ctx, const std::string& _name,
			Engine::Audio::AudioManager& _au, Engine::Scene::SceneManager& _sm,
			Engine::UISystem::UIManager& _ui, Manager::PlayerBulletManager& _pbm, 
			Manager::EnemyManager& _em, Manager::BulletManager& _bm);
		~Reimu() override = default;
		void Update(f32 d_t) override;
		void Render() override;
		void HandleInput() override;


	public:
		void SetOrb(i32 count);
		void RemoveOrb();
		std::pair<Engine::Maths::Vec2, float> GetGrazeData() const;
		void Shoot();
		Engine::Maths::Vec2 GetOrbPosition(int index) const;
	private:
		void InitOrbs();
		void InitEffect();
		void InitCheckBox();
		std::vector<float> GetOrbAnglesInDegrees(bool is_focused) const;
		void SetOrbTargetPositions(bool is_focused);
		void StartEffect();
	};
}