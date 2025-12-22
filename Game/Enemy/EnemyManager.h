#pragma once
#include "Enemy.h"
#include <map>
#include <functional> // ÐÂÔö

namespace Engine::Core {
	class Context;
}
namespace Engine::Audio {
	class AudioManager;
}
namespace Game::Entity {
	struct EnemyConfig {
		std::string source;
		SDL_FRect rect;
		Engine::Maths::Vec2 hit_box;
		Engine::Maths::Vec2 scale;
		std::vector<Engine::Render::Animation> anim;
	};
	class Player;
}

namespace Game::Manager {

	class EnemyManager {
	private:
		std::map<std::string, Entity::EnemyConfig> enemy_config;
		std::vector<std::unique_ptr<Entity::Enemy>> enemies;
		Engine::Core::Context& ctx;
		Engine::Audio::AudioManager& au;
	private:
		i32 index;
		using OnEnemyKilledCallback = std::function<void(const Engine::Maths::Vec2&, int)>;
		OnEnemyKilledCallback on_killed_cb;

	public:
		EnemyManager(Engine::Core::Context&, Engine::Audio::AudioManager&);
		~EnemyManager() = default;

	public:
		void Update(f32 d_t);
		void Render();

	public:
		void SetOnEnemyKilledCallback(OnEnemyKilledCallback cb) { on_killed_cb = cb; }
		int SpawnEnemy(const std::string& id, const Engine::Maths::Vec2& pos, i32 health, int drop_id);
		const std::vector<std::unique_ptr<Entity::Enemy>>& GetEnemies() const { return enemies; }
		bool CheckCollisions(Game::Entity::Player* player);
		Entity::Enemy* GetEnemyByIndex(int index);
		Entity::Enemy* GetNearestEnemyPosition(const Engine::Maths::Vec2& source_pos);
	};

	constexpr i32 MAX_ENEMY_SIZE = 64;
}