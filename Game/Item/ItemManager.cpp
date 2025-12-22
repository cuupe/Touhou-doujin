#include "ItemManager.h"
#include "Item.h"
#include "../Player/Player.h"
#include "../../Engine/Components/ColliderComponent.h"
#include "../../Engine/Components/TransformComponent.h"
#include "../../Engine/render.h"
#include "../../Engine/collider.h"
#include <fstream>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

namespace Game::Manager {
	using namespace Game::Items;
	using namespace Engine::Maths;
	using json = nlohmann::json;

	ItemManager::ItemManager(Engine::Core::Context& _ctx, Engine::Audio::AudioManager& _au)
		: ctx(_ctx), audio(_au)
	{
		items.resize(MAX_ITEM_SIZE);
		std::ifstream file("data/item_config.json");
		if (!file.is_open()) {
			spdlog::error("无法打开 item_config.json");
			return;
		}

		json j;
		try {
			file >> j;
			auto& config = j.at("item_config");
			source = config.at("source").get<std::string>();

			for (const auto& item_def : config.at("data")) {
				std::string type_str = item_def.at("type").get<std::string>();
				ItemType type = StringToItemType(type_str);

				if (type != ItemType::Unknown) {
					ItemConfig cfg;
					cfg.source = source;
					cfg.rect = {
						item_def.at("rect")[0].get<f32>(), item_def.at("rect")[1].get<f32>(),
						item_def.at("rect")[2].get<f32>(), item_def.at("rect")[3].get<f32>()
					};

					cfg.scale = { item_def.at("scale")[0].get<f32>(), item_def.at("scale")[1].get<f32>() };
					item_configs[type] = cfg;
				}
			}
		}
		catch (const json::parse_error& err) {
			throw std::runtime_error(err.what());
			return;
		}
	}

	Items::ItemType ItemManager::StringToItemType(const std::string& str) {
		if (str == "big_power") return ItemType::BigPower;
		if (str == "small_power") return ItemType::SmallPower;
		if (str == "quater_life") return ItemType::QuarterLife;
		if (str == "full_life") return ItemType::FullLife;
		if (str == "point") return ItemType::Point;
		return ItemType::Unknown;
	}

	Items::ItemData& ItemManager::GetAvailableItem() {
		for (int i = 0; i < MAX_ITEM_SIZE; ++i) {
			int current_idx = (index + i) % MAX_ITEM_SIZE;
			if (items[current_idx].state == IS_UNUSED) {
				index = current_idx;
				return items[index];
			}
		}
		index = (index + 1) % MAX_ITEM_SIZE;
		return items[index];
	}

	void ItemManager::SpawnItem(Items::ItemType type, const Vec2& pos) {
		if (item_configs.find(type) == item_configs.end()) return;

		ItemData& item = GetAvailableItem();
		const auto& config = item_configs[type];
		item.type = type;
		item.state = IS_FALLING;
		item.position = pos;
		item.rect = config.rect;
		item.scale = config.scale;
		item.hit_box = { 24.0f, 24.0f };

		item.magnet_speed = 0.0f;
		float rand_x = static_cast<float>(rand() % 100 - 50);
		float rand_y = static_cast<float>(rand() % 150 + 200);
		item.velocity = Vec2{ rand_x, -rand_y };
	}

	void ItemManager::SpawnItems(const Vec2& pos, const std::vector<std::pair<ItemType, int>>& items_to_spawn) {
		for (const auto& pair : items_to_spawn) {
			for (int i = 0; i < pair.second; ++i) {
				SpawnItem(pair.first, pos);
			}
		}
	}

	void ItemManager::RemoveAllItems() {
		for (auto& item : items) {
			item.state = IS_UNUSED;
		}
	}

	void ItemManager::Update(float dt) {
		for (auto& item : items) {
			if (item.state == IS_UNUSED) {
				continue;
			}
			if (item.state == IS_FALLING) {
				item.velocity.y += 500.0f * dt;
				if (item.velocity.y > 350.0f) item.velocity.y = 350.0f;
			}

			item.position.x += item.velocity.x * dt;
			item.position.y += item.velocity.y * dt;
			if (item.position.y > 1000.0f || item.position.x < -100.0f || item.position.x > 900.0f) {
				item.state = IS_UNUSED;
			}
		}
	}

	void ItemManager::CheckCollision(Game::Entity::Player* player) {
		if (!player) return;
		auto* p = player->GetPlayer();
		if (!p) return;

		auto* p_col = p->GetComponent<Engine::Core::Components::ColliderComponent>();
		auto* p_trans = p->GetComponent<Engine::Core::Components::TransformComponent>();
		if (!p_col || !p_trans) {
			return;
		}

		const Rect player_aabb = p_col->GetWorldAABB();
		const Vec2 player_pos = p_trans->GetPosition();
		bool global_magnet = (player_pos.y < 200.0f);

		for (auto& item : items) {
			if (item.state == IS_UNUSED) {
				continue;
			}
			float dx = player_pos.x - item.position.x;
			float dy = player_pos.y - item.position.y;
			float dist_sq = dx * dx + dy * dy;

			if (item.state != IS_MAGNETIZED) {
				if (global_magnet || dist_sq < 64.0f * 64.0f) {
					item.state = IS_MAGNETIZED;
					item.magnet_speed = 600.0f;
				}
			}

			if (item.state == IS_MAGNETIZED) {
				float len = std::sqrt(dist_sq);
				if (len > 0.001f) {
					item.magnet_speed += 1000.0f * 0.016f;
					item.velocity.x = (dx / len) * item.magnet_speed;
					item.velocity.y = (dy / len) * item.magnet_speed;
				}
			}
			Rect item_aabb = {
				item.position.x - item.hit_box.x / 2.0f,
				item.position.y - item.hit_box.y / 2.0f,
				item.hit_box.x,
				item.hit_box.y
			};

			if (Engine::Core::Collider::CheckRectOverlap(item_aabb, player_aabb)) {
				audio.PlaySFX("se_item00", "default");
				switch (item.type) {
				case ItemType::SmallPower:
					gamedata.power += 0.05f;
					gamedata.score += 10;
					break;
				case ItemType::BigPower:
					gamedata.power += 1.0f;
					gamedata.score += 1000;
					break;
				case ItemType::QuarterLife:
					gamedata.score += 500;
					gamedata.life += 1;
					if (gamedata.life > 35) {
						gamedata.life = 35;
					}
					break;
				case ItemType::FullLife:
					gamedata.life += 5;
					if (gamedata.life > 35) {
						gamedata.life = 35;
					}
					break;
				case ItemType::Point:
					gamedata.score += 100;
					break;
				default:
					break;
				}
				if (gamedata.power > 4.0f) gamedata.power = 4.0f;
				item.state = IS_UNUSED;
			}
		}
	}

	void ItemManager::Render() {
		Engine::Render::Renderer& renderer = ctx.GetRenderer();
		Engine::Resource::ResourceManager& res_manager = ctx.GetResourceManager();

		const Engine::Resource::TextureResource* texture = res_manager.GetTexture(source);
		if (!texture) return;

		for (const auto& item : items) {
			if (item.state == IS_UNUSED) continue;

			SDL_FRect dst_rect = {
				item.position.x - (item.rect.w * item.scale.x) / 2.0f,
				item.position.y - (item.rect.h * item.scale.y) / 2.0f,
				item.rect.w * item.scale.x,
				item.rect.h * item.scale.y
			};

			renderer.Draw(texture, &item.rect, &dst_rect, 0.0f, nullptr, nullptr, 0);
		}
	}
}