#pragma once
#include "../../Engine/maths.h"
#include "../GameData.h"
#include <vector>
#include <memory>
#include <functional> // 新增
#include <map>

namespace Engine::Core {
    class Context;
}
namespace Engine::Audio {
    class AudioManager;
}
namespace Game::Entity {
    class Enemy; // 只需要知道 Enemy 的存在
}

namespace Game::Manager {
    // 移除了 EnemyManager 和 ItemManager 的前置声明

    enum PlayerBulletState {
        PBS_UNUSED,
        PBS_ACTIVE,
        PBS_DESTROYING,
    };

    struct BulletAnimationFrame {
        SDL_FRect source_rect;
        f32 duration;
    };

    struct BulletConfig {
        std::string active_sprite_name;
        SDL_FRect active_rect;
        Engine::Maths::Vec2 hit_box;
        bool random;
        std::string destroy_sprite_name;
        std::vector<BulletAnimationFrame> destroy_anim_frames;
        f32 total_destroy_duration = 0.0f;
    };

    struct PlayerBulletData {
        Engine::Maths::Vec2 position;
        Engine::Maths::Vec2 v;
        Engine::Maths::Vec2 hit_box;
        f32 angle;
        f32 speed;
        f32 angle_acc = 0.0f;
        Entity::Enemy* target = nullptr;

        const BulletConfig* config_ptr = nullptr;

        PlayerBulletState state = PBS_UNUSED;
        f32 destroy_timer = 0.0f;
        bool is_active = false;
    };

    class PlayerBulletManager final {
    private:
        std::map<std::string, BulletConfig> bullet_config;
        std::vector<PlayerBulletData> player_bullets;
        Engine::Core::Context& ctx;
        Engine::Audio::AudioManager& au;
        const i32 MAX_BULLETS = 512;

        using OnEnemyKilledCallback = std::function<void(const Engine::Maths::Vec2&, int)>;
        OnEnemyKilledCallback on_enemy_killed_cb;

    private:
        PlayerBulletData& GetAvailableBullet();

    public:
        PlayerBulletManager(Engine::Core::Context& _ctx, Engine::Audio::AudioManager& _au);

        void Update(f32 d_t);
        void Render();

        void SetOnEnemyKilledCallback(OnEnemyKilledCallback cb) { on_enemy_killed_cb = cb; }

        void SpawnPlayerBullet(const std::string& type,
            const Engine::Maths::Vec2& position,
            f32 angle, f32 speed,
            f32 angle_acc = 0.0f,
            Entity::Enemy* target = nullptr);
        void CheckCollisions(const std::vector<std::unique_ptr<Entity::Enemy>>& enemies);
    };
}