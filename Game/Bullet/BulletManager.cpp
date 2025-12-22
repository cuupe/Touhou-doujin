#include "BulletManager.h"
#include "Bullet.h"
#include "../../Engine/animation.h"
#include "../../Engine/AudioManager.h"
#include "../../Engine/Components/ColliderComponent.h"
#include "../../Engine/maths.h"
#include "../../Engine/render.h"
#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>

namespace Game::Manager {
	using namespace Game::Bullets;
	using namespace Engine::Maths;
	using namespace Engine::Core::Collider;
	using namespace Engine::Render;
	using json = nlohmann::json;
	struct GlobalBreakAnimData {
		std::string Source;
		std::vector<BulletConfig::BulletAnimationFrame> Frames;
		f32 TotalDuration = 0.0f;
	};

	BulletManager::BulletManager(Engine::Core::Context& _ctx,
		Engine::Audio::AudioManager& _audio)
		:ctx(_ctx), audio(_audio), index(0)
	{
		std::ifstream file("data/bullet_config.json");
		json config_json;
		if (!file.is_open()) { bullets.resize(MAX_BULLET_SIZE); return; }
		try {
			file >> config_json;

			for (const auto& b : config_json.at("bullet_config")) {
				BulletConfig bc;
				bc.source = b.at("source").get<std::string>();
				bc.rect = { b.at("rect").at(0).get<f32>(), b.at("rect").at(1).get<f32>(), b.at("rect").at(2).get<f32>(), b.at("rect").at(3).get<f32>() };
				bc.hit_box = { b.at("hit_box").at(0).get<f32>(), b.at("hit_box").at(1).get<f32>() };
				for (const auto& v : b.at("offset")) {
					bc.offset.emplace_back(Engine::Maths::Vec2{ v.at(0).get<f32>(), v.at(1).get<f32>() });
					if (b.count("anim")) {
						const auto& anim_json = b.at("anim");
						int times = anim_json.at("times").get<int>();
						float offset = 0.0f;
						f32 duration = anim_json.at("duration").get<f32>();
						f32 rotation = anim_json.at("rotation").get<f32>();
						Vec2 scale = { anim_json.at("scale").at(0).get<f32>(), anim_json.at("scale").at(1).get<f32>() };
						f32 rect_offset = anim_json.at("rect_offset").get<f32>();
						while (times--) {
							bc.anim.emplace_back(BulletConfig::BulletAnimationFrame{ SDL_FRect{bc.rect.x + offset, bc.rect.y, bc.rect.w, bc.rect.h}, duration, scale, rotation });
							offset += rect_offset;
						}
					}
				}
				std::string id = b.at("id").get<std::string>();
				bullet_config.emplace(std::move(id), std::move(bc));
			}
			GlobalBreakAnimData break_anim_data;
			if (config_json.count("bullet_break_anim")) {
				const auto& break_json = config_json.at("bullet_break_anim");
				break_anim_data.Source = break_json.at("source").get<std::string>();
				f32 total_duration = 0.0f;
				for (const auto& re : break_json.at("anim")) {
					f32 duration = re.at("duration").get<f32>();
					break_anim_data.Frames.push_back(BulletConfig::BulletAnimationFrame{ SDL_FRect{re.at("frame").at(0).get<f32>(), re.at("frame").at(1).get<f32>(), re.at("frame").at(2).get<f32>(), re.at("frame").at(3).get<f32>()}, duration, Vec2{break_json.at("scale").at(0).get<f32>(), break_json.at("scale").at(1).get<f32>()}, 0.0f });
					total_duration += duration;
				}
				break_anim_data.TotalDuration = total_duration;
			}
			for (auto& pair : bullet_config) {
				pair.second.destroy_sprite_name = break_anim_data.Source;
				pair.second.destroy_anim_frames = break_anim_data.Frames;
				pair.second.total_destroy_duration = break_anim_data.TotalDuration;
			}
			bullets.resize(MAX_BULLET_SIZE);
		}
		catch (const json::parse_error& e) {
			spdlog::error("Bullet管理器初始化异常：{}", e.what());
			return;
		}
	}

	Bullets::BulletData& BulletManager::GetAvailableBullet() {
		for (auto& bullet : bullets) {
			if (bullet.state == BS_UNUSED) {
				return bullet;
			}
		}
		index = (index + 1) % MAX_BULLET_SIZE;
		return bullets[index];
	}

	void BulletManager::SpawnSingleBullet(const std::string& id, const Engine::Maths::Vec2& position,
		float speed, float angle, float speed_acc, float angle_acc,
		int sprite_offset)
	{
		if (bullet_config.find(id) == bullet_config.end()) return;
		Bullets::BulletData& data = GetAvailableBullet();
		if (data.state != BS_UNUSED) return;
		const BulletConfig* cfg = &bullet_config.at(id);
		data.config_ptr = cfg;
		data.state = BS_ACTIVE;
		data.destroy_timer = 0.0f;
		data.position = position;
		data.angle = angle;
		data.speed = speed;
		data.angle_acc = angle_acc;
		data.speed_acc = speed_acc;
		data.offset_pitch = sprite_offset;
		if (!cfg->anim.empty()) {
			data.current_anim_frame_index = 0;
			data.current_anim_timer = cfg->anim[0].duration;
		}
		else {
			data.current_anim_frame_index = 0;
			data.current_anim_timer = 0.0f;
		}
		if (sprite_offset < cfg->offset.size()) {
			data.hit_box = cfg->hit_box;
			const SDL_FRect& base_rect = cfg->anim.empty() ? cfg->rect : cfg->anim[data.current_anim_frame_index].source_rect;
			data.rect = { base_rect.x + cfg->offset[sprite_offset].x, base_rect.y + cfg->offset[sprite_offset].y, base_rect.w, base_rect.h };
		}
		else {
			data.rect = cfg->rect;
			data.hit_box = cfg->hit_box;
		}
		f32 angle_rad = DegToRad(data.angle);
		data.v.x = data.speed * cos(angle_rad);
		data.v.y = data.speed * sin(angle_rad);
	}

	void BulletManager::RemoveAllBullets(bool turn_into_items, int drop_id)
	{
		for (auto& bullet : bullets) {
			if (bullet.state == BS_ACTIVE) {
				bullet.state = BS_DESTROYING;
				if (bullet.config_ptr) {
					bullet.destroy_timer = bullet.config_ptr->total_destroy_duration;
				}
				else {
					bullet.destroy_timer = 0.001f;
				}

				if (turn_into_items && on_bullet_destroy_cb) {
					on_bullet_destroy_cb(bullet.position, drop_id);
				}
			}
		}
	}

	void BulletManager::DespawnBullet(Bullets::BulletData& bt)
	{
		bt.state = BS_UNUSED;
		bt.is_grazed = false;
	}

	void BulletManager::Update(f32 d_t) {
		for (auto& bullet : bullets) {
			if (bullet.state == BS_UNUSED) continue;
			if (bullet.state == BS_DESTROYING) {
				bullet.destroy_timer -= d_t;
				if (bullet.destroy_timer <= 0.0f) {
					bullet.state = BS_UNUSED;
				}
				continue;
			}
			const BulletConfig* config = bullet.config_ptr;
			if (config && !config->anim.empty()) {
				bullet.current_anim_timer -= d_t;
				if (bullet.current_anim_timer <= 0.0f) {
					bullet.current_anim_frame_index = (bullet.current_anim_frame_index + 1) % config->anim.size();
					const auto& next_frame = config->anim[bullet.current_anim_frame_index];
					bullet.current_anim_timer += next_frame.duration;
					bullet.rect = SDL_FRect{ 
						next_frame.source_rect.x + bullet.config_ptr->offset[bullet.offset_pitch].x,
					next_frame.source_rect.y + bullet.config_ptr->offset[bullet.offset_pitch].y, 
						next_frame.source_rect.w, next_frame.source_rect.h };
				}
			}
			if (bullet.angle_acc != 0.0f) bullet.angle += bullet.angle_acc * d_t;
			bullet.speed += bullet.speed_acc * d_t;
			if (bullet.restrict_speed && bullet.speed > bullet.max_speed) bullet.speed = bullet.max_speed;
			f32 angle_rad = DegToRad(bullet.angle);
			bullet.v.x = bullet.speed * cos(angle_rad);
			bullet.v.y = bullet.speed * sin(angle_rad);
			bullet.position.x += bullet.v.x * d_t;
			bullet.position.y += bullet.v.y * d_t;
			if (bullet.position.y < -100.0 || bullet.position.y > 1000.0 ||
				bullet.position.x < -100.0 || bullet.position.x > 900.0)
			{
				bullet.state = BS_DESTROYING;
				if (bullet.config_ptr) bullet.destroy_timer = bullet.config_ptr->total_destroy_duration;
				else bullet.destroy_timer = 0.001f;
			}
		}
	}

	void BulletManager::CheckCollisions(Entity::Player* player) {
		Engine::Core::GameObject* player_go = player->GetPlayer();
		if (!player_go) {
			return;
		}

		auto player_collider_comp = player_go->GetComponent<Engine::Core::Components::ColliderComponent>();
		if (!player_collider_comp || !player_collider_comp->IsActive()) {
			return;
		}
		const Engine::Maths::Rect player_aabb = player_collider_comp->GetWorldAABB();
		auto [player_graze_pos, player_graze_radius] = player->GetGrazeData();
		float graze_sq_dist = player_graze_radius * player_graze_radius;

		for (auto& bullet : bullets) {
			if (bullet.state != BS_ACTIVE) {
				continue;
			}
			f32 half_w = bullet.hit_box.x / 2.0f;
			f32 half_h = bullet.hit_box.y / 2.0f;
			Engine::Maths::Rect bullet_aabb = {
				bullet.position.x - half_w,
				bullet.position.y - half_h,
				bullet.hit_box.x,
				bullet.hit_box.y
			};
			if (Engine::Core::Collider::CheckRectOverlap(bullet_aabb, player_aabb)) {
				bullet.state = BS_DESTROYING;
				if (player->IsMuteki()) {
					DespawnBullet(bullet);
					break;
				}
				player->SetDead();
				RemoveAllBullets(true);
				break;
			}
			else if (!bullet.is_grazed && !player->IsMuteki()) {
				float dx = bullet.position.x - player_graze_pos.x;
				float dy = bullet.position.y - player_graze_pos.y;
				float dist_sq = dx * dx + dy * dy;
				float bullet_radius = bullet.hit_box.x / 2.0f;
				float check_dist = player_graze_radius + bullet_radius;

				if (dist_sq < check_dist * check_dist) {
					bullet.is_grazed = true;
					audio.PlaySFX("se_graze", "common");
					gamedata.score += 100;
					gamedata.graze += 1;
				}
			}
		}
	}

	void BulletManager::Render() {
		Engine::Render::Renderer& renderer = ctx.GetRenderer();
		Engine::Resource::ResourceManager& res_manager = ctx.GetResourceManager();
		for (auto& bullet : bullets) {
			if (bullet.state == BS_UNUSED || !bullet.config_ptr) continue;
			const BulletConfig* config = bullet.config_ptr;
			const Engine::Resource::TextureResource* texture_res = nullptr;
			const SDL_FRect* source_rect = nullptr;
			f32 draw_angle = 0.0f;
			SDL_FRect dst_rect;
			if (bullet.state == BS_ACTIVE) {
				texture_res = res_manager.GetTexture(config->source);
				source_rect = &bullet.rect;
				draw_angle = bullet.angle + 90.0f;
			}
			else if (bullet.state == BS_DESTROYING) {
				texture_res = res_manager.GetTexture(config->destroy_sprite_name);
				draw_angle = 90.0f;
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
					source_rect->w * 2.0f,
					source_rect->h * 2.0f
				};
				renderer.Draw(texture_res, source_rect, &dst_rect, draw_angle, nullptr, nullptr, 0);
			}
		}
	}
}