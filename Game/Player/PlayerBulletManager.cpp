#include "../../Engine/AudioManager.h"
#include "../../Engine/maths.h"
#include "../../Engine/context.h"
#include "../../Engine/render.h"
#include "../../Engine/resources.h"
#include "../../Engine/collider.h"
#include "../../Engine/Components/TransformComponent.h"
#include "../../Engine/Components/ColliderComponent.h"
#include "PlayerBulletManager.h"
#include "../Enemy/Enemy.h"
#include "../Enemy/EnemyComponent.h"
#include <fstream>
#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>

namespace Game::Manager {
    using namespace Engine::Maths;
    using namespace Engine::Resource;
    using namespace Engine::Render;
    using json = nlohmann::json;

    PlayerBulletManager::PlayerBulletManager(Engine::Core::Context& _ctx, Engine::Audio::AudioManager& _au)
        : ctx(_ctx), au(_au) {
        std::ifstream file("data/player_config.json");
        json config_data;
        if (file.is_open()) {
            try {
                file >> config_data;
                const auto& player_data = config_data.at("player_config").at(0).at("player_1");
                const auto& bullet_configs_json = player_data.at("bullet_config");
                const std::string common_source = player_data.at("source").get<std::string>();
                for (const auto& bullet_json : bullet_configs_json) {
                    BulletConfig config;
                    const std::string type_id = bullet_json.at("bullet_type").get<std::string>();
                    config.active_sprite_name = common_source;
                    const auto& active_rect_arr = bullet_json.at("rect");
                    config.active_rect = {
                        active_rect_arr.at(0).get<f32>(),
                        active_rect_arr.at(1).get<f32>(),
                        active_rect_arr.at(2).get<f32>(),
                        active_rect_arr.at(3).get<f32>()
                    };
                    const auto& hit_box = bullet_json.at("hit_box");
                    config.hit_box = {
                        hit_box.at(0).get<f32>(),
                        hit_box.at(1).get<f32>()
                    };
                    config.destroy_sprite_name = common_source;
                    config.total_destroy_duration = 0.0f;
                    const auto& destroy_frames_json = bullet_json.at("destroy_anim").at("anim_frames");
                    config.random = bullet_json.at("destroy_anim").at("random").get<bool>();
                    for (const auto& frame_json : destroy_frames_json) {
                        BulletAnimationFrame frame;
                        const auto& frame_rect_arr = frame_json.at("frame");
                        frame.source_rect = {
                            frame_rect_arr.at(0).get<f32>(),
                            frame_rect_arr.at(1).get<f32>(),
                            frame_rect_arr.at(2).get<f32>(),
                            frame_rect_arr.at(3).get<f32>()
                        };
                        frame.duration = frame_json.at("duration").get<f32>();
                        config.total_destroy_duration += frame.duration;
                        config.destroy_anim_frames.push_back(std::move(frame));
                    }
                    bullet_config.emplace(type_id, std::move(config));
                }
            }
            catch (const std::exception& err) {
                throw std::runtime_error(err.what());
                return;
            }
        }
        player_bullets.resize(MAX_BULLETS);
    }

    PlayerBulletData& PlayerBulletManager::GetAvailableBullet() {
        for (auto& bullet : player_bullets) {
            if (!bullet.is_active) {
                return bullet;
            }
        }
        return player_bullets[0];
    }

    void PlayerBulletManager::SpawnPlayerBullet(const std::string& type,
        const Engine::Maths::Vec2& position,
        f32 angle, f32 speed,
        f32 angle_acc,
        Entity::Enemy* target) {
        if (bullet_config.find(type) == bullet_config.end()) {
            return;
        }

        PlayerBulletData& bullet = GetAvailableBullet();

        const BulletConfig* cfg = &bullet_config.at(type);
        bullet.config_ptr = cfg;
        bullet.hit_box = cfg->hit_box;
        bullet.is_active = true;
        bullet.state = PBS_ACTIVE;
        bullet.destroy_timer = 0.0f;
        bullet.position = position;
        bullet.angle = angle;
        bullet.speed = speed;
        bullet.angle_acc = angle_acc;
        bullet.target = target;

        f32 angle_rad = angle * _PI / 180.0f;
        bullet.v.x = std::cos(angle_rad) * speed;
        bullet.v.y = -std::sin(angle_rad) * speed;
    }

    void PlayerBulletManager::CheckCollisions(const std::vector<std::unique_ptr<Entity::Enemy>>& enemies) {
        for (auto& bullet : player_bullets) {
            if (bullet.state != PBS_ACTIVE) {
                continue;
            }

            f32 b_w = bullet.hit_box.x;
            f32 b_h = bullet.hit_box.y;

            Engine::Maths::Rect bullet_aabb = {
                bullet.position.x - b_w / 2.0f,
                bullet.position.y - b_h / 2.0f,
                b_w,
                b_h
            };

            for (auto& enemy_ptr : enemies) {
                auto enemy = enemy_ptr->GetGameObject();
                if (!enemy) continue;

                auto ene = enemy->GetComponent<Game::Component::EnemyComponent>();
                if (!ene || !ene->GetEnemyData().is_active) continue;

                auto collider = enemy->GetComponent<Engine::Core::Components::ColliderComponent>();
                if (!collider || !collider->IsActive()) continue;

                const Engine::Maths::Rect enemy_aabb = collider->GetWorldAABB();
                if (Engine::Core::Collider::CheckRectOverlap(bullet_aabb, enemy_aabb)) {
                    bullet.state = PBS_DESTROYING;
                    bullet.destroy_timer = bullet.config_ptr->total_destroy_duration;

                    if (ene) {
                        au.PlaySFX("se_damage00", "damage");
                        gamedata.score += 10;
                        if (ene->Damage(10)) {
                            if (on_enemy_killed_cb) {
                                int drop_id = ene->GetEnemyData().drop_id;
                                if (drop_id > 0) {
                                    on_enemy_killed_cb(ene->GetEnemyData().position, drop_id);
                                }
                            }
                        }
                    }
                    break;
                }
            }
        }
    }

    void PlayerBulletManager::Update(f32 d_t) {
        for (auto& bullet : player_bullets) {
            if (!bullet.is_active) continue;
            if (bullet.state == PBS_DESTROYING) {
                bullet.destroy_timer -= d_t;
                if (bullet.destroy_timer <= 0.0f) {
                    bullet.state = PBS_UNUSED;
                    bullet.is_active = false;
                    bullet.target = nullptr;
                }
                continue;
            }
            if (bullet.target != nullptr) {
                auto& data = bullet.target->GetEnemyComponent()->GetEnemyData();
                if (data.is_active) {
                    Vec2 direction_vec;
                    Vec2 pos = bullet.target->GetGameObject()->GetComponent<TransformComponent>()->GetPosition();
                    direction_vec.x = pos.x - bullet.position.x;
                    direction_vec.y = pos.y - bullet.position.y;
                    f32 target_angle_rad = std::atan2(-direction_vec.y, direction_vec.x);
                    f32 current_angle_rad = bullet.angle * _PI / 180.0f;
                    f32 angle_diff = target_angle_rad - current_angle_rad;
                    if (angle_diff > _PI) angle_diff -= 2 * _PI;
                    if (angle_diff < -_PI) angle_diff += 2 * _PI;
                    f32 max_turn_rad = bullet.angle_acc * _PI / 180.0f * d_t;
                    angle_diff = std::clamp(angle_diff, -max_turn_rad, max_turn_rad);
                    current_angle_rad += angle_diff;
                    bullet.angle = current_angle_rad * 180.0f / _PI;
                }
            }
            else if (bullet.angle_acc != 0.0f) {
                bullet.angle += bullet.angle_acc * d_t;
            }

            f32 angle_rad = bullet.angle * _PI / 180.0f;
            bullet.v.x = std::cos(angle_rad) * bullet.speed;
            bullet.v.y = -std::sin(angle_rad) * bullet.speed;
            bullet.position.x += bullet.v.x * d_t;
            bullet.position.y += bullet.v.y * d_t;
            if (bullet.position.y < -80.0 || bullet.position.y > 1000.0 ||
                bullet.position.x < -100.0 || bullet.position.x > 900.0) {
                bullet.state = PBS_DESTROYING;
                if (bullet.config_ptr) bullet.destroy_timer = bullet.config_ptr->total_destroy_duration;
                else bullet.destroy_timer = 0.001f;
            }
        }
    }

    void PlayerBulletManager::Render() {
        Renderer& renderer = ctx.GetRenderer();
        ResourceManager& res_manager = ctx.GetResourceManager();
        for (auto& bullet : player_bullets) {
            if (!bullet.is_active || !bullet.config_ptr) {
                continue;
            }
            const BulletConfig* config = bullet.config_ptr;
            TextureResource* texture_res = nullptr;
            const SDL_FRect* source_rect = nullptr;
            f32 draw_angle = 0.0f;
            float random_scale = 1.0f;
            SDL_FRect dst_rect;
            if (bullet.state == PBS_ACTIVE) {
                texture_res = res_manager.GetTexture(config->active_sprite_name);
                source_rect = &config->active_rect;
                draw_angle = bullet.angle + 180.0f;
            }
            else if (bullet.state == PBS_DESTROYING) {
                texture_res = res_manager.GetTexture(config->destroy_sprite_name);
                if (config->random) {
                    draw_angle = Engine::Maths::GetRandomFloat(0.0f, 180.0f);
                    random_scale *= Engine::Maths::GetRandomFloat(0.0f, 2.0f);
                }
                f32 elapsed_time = config->total_destroy_duration - bullet.destroy_timer;
                f32 cumulative_duration = 0.0f;
                for (const auto& frame : config->destroy_anim_frames) {
                    cumulative_duration += frame.duration;
                    if (elapsed_time <= cumulative_duration) {
                        source_rect = &frame.source_rect;
                        break;
                    }
                }
            }
            if (texture_res && source_rect) {
                dst_rect = {
                    bullet.position.x - source_rect->w,
                    bullet.position.y - source_rect->h,
                    source_rect->w * 2.0f * random_scale,
                    source_rect->h * 2.0f * random_scale
                };
                renderer.Draw(texture_res, source_rect, &dst_rect, draw_angle, nullptr, nullptr, 0);
            }
        }
    }
}