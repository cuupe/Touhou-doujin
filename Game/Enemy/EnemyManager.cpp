#include "EnemyManager.h"
#include "../../Engine/AudioManager.h"
#include "../Player/Player.h"
#include "EnemyComponent.h"
#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>

namespace Game::Manager {
	using namespace Game::Entity;

	EnemyManager::EnemyManager(Engine::Core::Context& _ctx, Engine::Audio::AudioManager& _au)
		:ctx(_ctx), au(_au)
	{
		index = 0;
		enemies.reserve(MAX_ENEMY_SIZE);

		using json = nlohmann::json;
		std::ifstream file("data/enemy_config.json");
		json config_json;
		try {
			file >> config_json;
		}
		catch (const json::parse_error& e) {
			enemies.resize(MAX_ENEMY_SIZE);
			return;
		}

		for (const auto& c : config_json["enemy_config"]) {
			Entity::EnemyConfig ecf;
			ecf.source = c["source"];
			ecf.rect = { c["rect"][0], c["rect"][1], c["rect"][2], c["rect"][3] };
			ecf.scale = { c["scale"][0], c["scale"][1] };
			ecf.hit_box = { c["hit_box"][0], c["hit_box"][1] };
			for (const auto& a : c["anim"]) {
				Engine::Render::Animation anim(std::string(a["name"]),
					a["anim_type"] == "loop" ? true : false);
				for (const auto& frame : a["anim_frames"]) {
					anim.AddFrame({ frame["frame"][0], frame["frame"][1],
						frame["frame"][2], frame["frame"][3] }, frame["duration"],
						{ c["scale"][0], c["scale"][1] }, frame["rotation"]);
				}
				ecf.anim.push_back(std::move(anim));
			}
			enemy_config.emplace(std::string(c["id"]), std::move(ecf));
		}
		auto& initiate = enemy_config.begin()->second;
		for (int i = 0; i < MAX_ENEMY_SIZE; ++i) {
			Entity::EnemyData ed;
			ed.sprite_name = initiate.source;
			ed.health = 1;
			ed.position = { -200.0f, -200.0f };
			ed.rect = initiate.rect;
			ed.hit_box = initiate.hit_box;
			ed.scale = initiate.scale;
			ed.enable_body_collision = true;
			ed.is_active = false;
			ed.is_boss = false;
			enemies.push_back(std::make_unique<Entity::Enemy>(std::move(ed), _ctx, _au));
		}
	}

	void EnemyManager::Update(f32 d_t)
	{
		for (int i = 0; i < MAX_ENEMY_SIZE; ++i) {
			auto& data = enemies[i]->GetGameObject()->GetComponent<Game::Component::EnemyComponent>()->GetEnemyData();
			if (data.is_active) {
				auto& pos = data.position;
				if (pos.x > 1200.0f || pos.x < -200.0f || pos.y > 1000.0f || pos.y < -100.0f) {
					data.is_active = false;
				}

				enemies[i]->Update(d_t);
			}
		}
	}

	void EnemyManager::Render()
	{
		for (int i = 0; i < MAX_ENEMY_SIZE; ++i) {
			auto& data = enemies[i]->GetGameObject()->GetComponent<Game::Component::EnemyComponent>()->GetEnemyData();
			if (data.is_active) {
				enemies[i]->Render();
			}
		}
	}

	int EnemyManager::SpawnEnemy(const std::string& id, const Engine::Maths::Vec2& pos, i32 health, int drop_id)
	{
		if (enemy_config.find(id) == enemy_config.end()) {
			spdlog::error("找不到敌方资源");
			return -1;
		}

		auto e = enemies[index].get();
		auto& data = e->GetGameObject()->GetComponent<Game::Component::EnemyComponent>()->GetEnemyData();
		auto& config = enemy_config[id];

		data.position = pos;
		data.health = health;
		data.ecf = &config;
		data.hit_box = config.hit_box;
		data.sprite_name = config.source;
		data.rect = config.rect;
		data.scale = config.scale;
		data.is_active = true;
		data.is_boss = false;
		data.drop_id = drop_id;

		auto trans = e->GetGameObject()->GetComponent<Engine::Core::Components::TransformComponent>();
		trans->SetPosition(data.position);
		trans->SetScale(data.scale);
		auto sprite = e->GetGameObject()->GetComponent<Engine::Core::Components::SpriteComponent>();
		if (sprite->GetSpriteTextureName() != config.source) {
			sprite->SetSprite(std::move(Engine::Render::Sprite(data.sprite_name)));
		}
		sprite->SetRect(data.rect);
		auto colli = e->GetGameObject()->GetComponent<Engine::Core::Components::ColliderComponent>();
		colli->SetCollider(std::make_unique<Engine::Core::Collider::AABBCollider>(data.hit_box));
		colli->SetAlignment(Engine::Maths::Align::CENTER);
		auto anim = e->GetGameObject()->GetComponent<Engine::Core::Components::AnimationComponent>();
		anim->RemoveAllAnimations();
		if (config.anim.size()) {
			for (const auto& a : config.anim) {
				anim->AddAnimation(std::make_unique<Engine::Render::Animation>(a));
			}
			anim->PlayAnimation("default");
		}
		int temp = index;
		++index;
		if (index >= MAX_ENEMY_SIZE) {
			index = 0;
		}

		return temp;
	}

	bool EnemyManager::CheckCollisions(Game::Entity::Player* player) {
		Engine::Core::GameObject* player_go = player->GetPlayer();
		if (!player_go) return false;

		auto* player_collider = player_go->GetComponent<Engine::Core::Components::ColliderComponent>();
		if (!player_collider || !player_collider->IsActive()) return false;

		const Engine::Maths::Rect player_aabb = player_collider->GetWorldAABB();

		for (auto& enemy_ptr : enemies) {
			auto* enemy_go = enemy_ptr->GetGameObject();
			if (!enemy_go) continue;
			auto* enemy_comp = enemy_go->GetComponent<Game::Component::EnemyComponent>();
			if (!enemy_comp || !enemy_comp->GetEnemyData().enable_body_collision ||
				!enemy_comp->GetEnemyData().is_active) {
				continue;
			}
			auto* enemy_collider = enemy_go->GetComponent<Engine::Core::Components::ColliderComponent>();
			if (!enemy_collider) continue;

			const Engine::Maths::Rect enemy_aabb = enemy_collider->GetWorldAABB();
			if (!player->IsMuteki() && Engine::Core::Collider::CheckRectOverlap(enemy_aabb, player_aabb)) {
				for (auto& e : enemies) {
					auto* comp = e->GetEnemyComponent();
					comp->Damage(500);
				}

				player->SetDead();
				gamedata.score -= 1000;
				if (gamedata.score < 0) {
					gamedata.score = 0;
				}

				return true;
			}
		}
		return false;
	}

	Entity::Enemy* EnemyManager::GetEnemyByIndex(int idx)
	{
		if (idx < 0 || idx >= enemies.size()) {
			return nullptr;
		}
		auto& ptr = enemies.at(idx);
		if (!ptr->GetEnemyComponent()->GetEnemyData().is_active) {
			return nullptr;
		}
		return ptr.get();
	}

	Entity::Enemy* EnemyManager::GetNearestEnemyPosition(const Engine::Maths::Vec2& source_pos)
	{
		Entity::Enemy* nearest_target = nullptr;
		float min_dist = std::numeric_limits<float>::max();

		for (const auto& enemy_ptr : enemies) {
			auto* enemy_go = enemy_ptr->GetGameObject();
			if (!enemy_go) continue;

			auto* enemy_comp = enemy_go->GetComponent<Game::Component::EnemyComponent>();
			if (!enemy_comp) {
				continue;
			}
			auto& data = enemy_comp->GetEnemyData();
			if (!data.is_active) {
				continue;
			}

			float dx = data.position.x - source_pos.x;
			float dy = data.position.y - source_pos.y;
			float dist_sq = dx * dx + dy * dy;

			if (dist_sq < min_dist) {
				min_dist = dist_sq;
				nearest_target = enemy_ptr.get();
			}
		}

		return nearest_target;
	}

}