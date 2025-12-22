#include "Player.h"
#include "../Engine/GameObject.h"
#include "../Engine/AudioManager.h"
#include "../Engine/Components/TransformComponent.h"
#include "../Engine/Components/SpriteComponent.h"
#include "../Engine/Components/ColliderComponent.h"
#include "../Engine/Components/AnimationComponent.h"
#include "../Engine/Components/PlayerComponent.h"
#include "PlayerBulletManager.h"
#include "../Engine/UIManager.h"
#include "../Scene/UIPanel/OverPanel.h"
#include "../Engine/EffectManager.h"
#include <nlohmann/json.hpp>
namespace Game::Entity {
	Player::Player(Engine::Core::Context& _ctx, const std::string& _name,
        Engine::Audio::AudioManager& _au, Engine::Scene::SceneManager& _sm, 
        Engine::UISystem::UIManager& _ui, Game::Manager::PlayerBulletManager& _pbm, 
        Manager::EnemyManager& _em, Manager::BulletManager& _bm)
        :ctx(_ctx), pbm(_pbm), au(_au), em(_em), ui(_ui), sm(_sm), bm(_bm)
	{
		using namespace Engine::Core::Components;
        try {


            std::ifstream file("data/player_config.json");
            using json = nlohmann::json;
            json config_json;
            file >> config_json;

            for (const auto& p : config_json["player_config"]) {
                for (const auto& player_entry : p.items()) {
                    if (player_entry.key() == "player_1") {
                        const auto& player_data = player_entry.value();

                        role = std::make_unique<Engine::Core::GameObject>("", "player");
                        role->AddComponent<TransformComponent>(Vec2{ 512.0f, 650.0f },
                            Vec2{ player_data["scale"][0], player_data["scale"][1] });
                        role->AddComponent<SpriteComponent>(player_data["source"],
                            ctx.GetResourceManager(), SDL_FRect{ 0.0f, 0.0f, 32.0f, 48.0f },
                            Align::CENTER);
                        role->AddComponent<ColliderComponent>(
                            std::make_unique<Engine::Core::Collider::AABBCollider>(
                                Vec2{ player_data["hit_box"][0], player_data["hit_box"][1] }),
                            Align::CENTER);

                        role->AddComponent<AnimationComponent>();
                        auto anim_component = role->GetComponent<Engine::Core::Components::AnimationComponent>();
                        for (const auto& anim : player_data["anim"]) {
                            std::string anim_name = anim["name"];
                            bool is_loop = (anim["anim_type"] == "loop");

                            Animation animation(anim_name, is_loop);

                            for (const auto& frame : anim["anim_frames"]) {
                                SDL_FRect frame_rect = {
                                    static_cast<float>(frame["frame"][0]),
                                    static_cast<float>(frame["frame"][1]),
                                    static_cast<float>(frame["frame"][2]),
                                    static_cast<float>(frame["frame"][3])
                                };
                                float duration = frame["duration"];

                                animation.AddFrame(frame_rect, duration);
                            }

                            anim_component->AddAnimation(std::make_unique<Animation>(animation));
                        }
                    }
                }
            }
            role->GetComponent<Engine::Core::Components::AnimationComponent>()->PlayAnimation("idle");
            role->AddComponent<Engine::Core::Components::PlayerComponent>(500.0f);
        }
        catch (const std::exception& err) {
            spdlog::error("发生错误：{}", err.what());
            return;
        }
	}

    //返回弧度制
	f32 Player::AngleToPlayer(const Vec2& pos)
	{
		auto& this_pos = role->GetComponent<Engine::Core::Components::TransformComponent>()->GetPosition();
		Vec2 dir = this_pos - pos;

		f32 angle = Engine::Maths::Vec2ToAngle(dir);
		return angle;
	}

    void Player::SetDead()
    {
        au.PlaySFX("se_pldead00", "dead");
        Engine::Render::Effect::EffectManager::Flash(role.get(), 5.0f);
        role->GetComponent<Engine::Core::Components::TransformComponent>()->SetPosition(
            Vec2{437.5f, 875.0f});
        is_muteki = true;
        gamedata.score -= 1500;
        if (gamedata.score < 0) {
            gamedata.score = 0;
        }
        if (gamedata.life < 5) {
            Reset();
            gamedata.life = 0;
            ui.RequestPushPanel(std::make_unique<UI::OverPanel>("gameover", ui));
            is_over = true;
            return;
        }
        gamedata.life -= 5;
    }

    void Player::Shoot()
    {
        au.PlaySFX("se_plst00", "shoot");
    }

    std::pair<Engine::Maths::Vec2, float> Player::GetGrazeData() const
    {
        if (role) {
            auto pos = role->GetComponent<Engine::Core::Components::TransformComponent>()->GetPosition();
            return { pos, 20.0f };
        }
        return { {0,0}, 0.0f };
    }

    void Player::Reset()
    {
        role->GetComponent<Engine::Core::Components::TransformComponent>()->SetPosition(
            Vec2{ 437.5f, 875.0f });
        is_muteki = false;
        is_over = false;
    }

    void Player::Update(f32 d_t) {
        role->Update(d_t, ctx);
        Vec2 pos = role->GetComponent<TransformComponent>()->GetPosition();
        if(pos.x >= 760.0f || pos.x <= 65.0f || pos.y >= 875.0f || pos.y <= 65.0f) {
            role->GetComponent<TransformComponent>()->SetPosition(std::clamp(pos.x, 65.0f, 760.0f), 
                std::clamp(pos.y, 65.0f, 875.0f));
        }
        if (is_muteki) {
            count_time += d_t;
            if (count_time > muteki_time) {
                count_time = 0;
                is_muteki = false;
            }
        }

    }

    void Player::HandleInput() {
        role->HandleInput(ctx);
    }

    void Player::Render() {
        role->Render(ctx);
    }
}


