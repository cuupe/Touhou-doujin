#include "Reimu.h"
#include "../../GameData.h"
#include "../Engine/Components/TransformComponent.h"
#include "../Engine/Components/SpriteComponent.h"
#include "../Engine/Components/PlayerComponent.h"
#include "../Engine/Components/ColliderComponent.h"
#include "../PlayerBulletManager.h"
#include "../../Bullet/BulletManager.h"
#include "../../Enemy/EnemyManager.h"
#include "../Engine/InputManager.h"
#include "../Engine/AudioManager.h"
#include <cmath>
#include <algorithm>
namespace Game::Player {
    Reimu::Reimu(Engine::Core::Context& _ctx, const std::string& _name,
        Engine::Audio::AudioManager& _au, Engine::Scene::SceneManager& _sm,
        Engine::UISystem::UIManager& _ui, Manager::PlayerBulletManager& _pbm,
        Manager::EnemyManager& _em, Manager::BulletManager& _bm)
        : Game::Entity::Player(_ctx, _name, _au, _sm, _ui, _pbm, _em, _bm)
    {
        role->GetComponent<Engine::Core::Components::PlayerComponent>()->SetSpeed(550.0f);
        InitCheckBox();
        InitEffect();
        InitOrbs();

        current_orb_count = 1;
        SetOrbTargetPositions(false);

        for (i32 i = 0; i < current_orb_count; ++i) {
            auto& state = orb_states[i];
            orbs[i]->GetComponent<Engine::Core::Components::TransformComponent>()->SetPosition(state.target_pos);
            state.current_pos = state.target_pos;
            state.transition_timer = state.TRANSITION_DURATION;
            state.is_transitioning = false;
            state.last_focus_mode = false;
        }
    }

    void Reimu::SetOrb(i32 count) {
        int old_count = current_orb_count;
        current_orb_count = std::clamp(count, 0, 4);

        if (current_orb_count > old_count) {
            bool is_focused = role->GetComponent<Engine::Core::Components::PlayerComponent>()->GetIsSlow();
            SetOrbTargetPositions(is_focused);
            for (int i = 0; i < current_orb_count; ++i) {
                auto& state = orb_states[i];
                state.current_pos = orbs[i]->GetComponent<Engine::Core::Components::TransformComponent>()->GetPosition();
                state.transition_timer = 0.0f;
            }
        }
    }

    void Reimu::RemoveOrb() {
        using namespace Engine::Core::Components;

        int old_count = current_orb_count;
        current_orb_count = std::clamp(current_orb_count - 1, 1, 4);

        if (current_orb_count < old_count) {
            if (old_count - 1 < orbs.size() && orbs[old_count - 1]) {
                orbs[old_count - 1]->GetComponent<SpriteComponent>()->SetHidden(true);
            }
            bool is_focused = role->GetComponent<PlayerComponent>()->GetIsSlow();
            SetOrbTargetPositions(is_focused);

            for (int i = 0; i < current_orb_count; ++i) {
                auto& state = orb_states[i];
                state.current_pos = orbs[i]->GetComponent<TransformComponent>()->GetPosition();

                state.transition_timer = 0.0f;
                state.is_transitioning = true;
            }
        }
    }

    std::pair<Engine::Maths::Vec2, float> Reimu::GetGrazeData() const {
        return {
            role->GetComponent<Engine::Core::Components::TransformComponent>()->GetPosition(),
            GRAZE_RADIUS
        };
    }

    void Reimu::Shoot() {
        Player::Shoot();
        using Engine::Core::Components::TransformComponent;
        using Engine::Core::Components::PlayerComponent;

        bool is_focused = this->role->GetComponent<PlayerComponent>()->GetIsSlow();

        Vec2 player_pos = this->role->GetComponent<TransformComponent>()->GetPosition();
        Vec2 mock_target_pos = { player_pos.x, 150.0f };
        Vec2* target_ptr = &mock_target_pos;
        if (shoot_type) {
            pbm.SpawnPlayerBullet(
                reimu_main_shot,
                { player_pos.x - 10.0f, player_pos.y },
                90.0f, main_shot_speed
            );
            pbm.SpawnPlayerBullet(
                reimu_main_shot,
                { player_pos.x + 10.0f, player_pos.y },
                90.0f, main_shot_speed
            );
        }
        else {
            pbm.SpawnPlayerBullet(
                reimu_curve_shot,
                { player_pos.x - 10.0f, player_pos.y },
                90.0f, main_shot_speed
            );
            pbm.SpawnPlayerBullet(
                reimu_curve_shot,
                { player_pos.x + 10.0f, player_pos.y },
                90.0f, main_shot_speed
            );
        }
        shoot_type = !shoot_type;

        for (i32 i = 0; i < current_orb_count; ++i) {
            Vec2 orb_pos = GetOrbPosition(i);
            Entity::Enemy* target = em.GetNearestEnemyPosition(orb_pos);
            f32 turn_speed = (target != nullptr) ? 3600.0f : 0.0f;

            pbm.SpawnPlayerBullet(
                orb_support_shot,
                orb_pos,
                90.0f,
                orb_support_speed,
                turn_speed,
                target
            );
        }
    }


    Engine::Maths::Vec2 Reimu::GetOrbPosition(int index) const
    {
        if (!orbs[index]->HasComponent<Engine::Core::Components::TransformComponent>()) {
            throw std::runtime_error("阴阳玉不包含位置组件");
            return { 0.0f, 0.0f };
        }
        return orbs[index]->GetComponent<Engine::Core::Components::TransformComponent>()->GetPosition();
    }

    std::vector<float> Reimu::GetOrbAnglesInDegrees(bool is_focused) const {
        std::vector<float> degrees;

        if (is_focused) {
            if (current_orb_count == 1) {
                degrees = { 90.0f };
            }
            else if (current_orb_count == 2) {
                degrees = { 60.0f, 120.0f };
            }
            else if (current_orb_count == 3) {
                degrees = { 60.0f, 90.0f, 120.0f };
            }
            else if (current_orb_count == 4) {
                degrees = { 45.0f, 75.0f, 105.0f, 135.0f };
            }
        }
        else {
            if (current_orb_count == 1) {
                degrees = { 90.0f };
            }
            else if (current_orb_count == 2) {
                degrees = { 240.0f, 300.0f };
            }
            else if (current_orb_count == 3) {
                degrees = { 220.0f, 270.0f, 320.0f };
            }
            else if (current_orb_count == 4) {
                degrees = { 180.0f, 240.0f, 300.0f, 0.0f };
            }
        }
        return degrees;
    }

    void Reimu::SetOrbTargetPositions(bool is_focused) {
        using namespace Engine::Core::Components;

        if (!role || current_orb_count == 0) {
            return;
        }

        Engine::Maths::Vec2 player_pos = role->GetComponent<TransformComponent>()->GetPosition();
        float target_radius = is_focused ? ORB_RADIUS_FOCUSED : ORB_RADIUS_NORMAL;
        std::vector<float> degrees = GetOrbAnglesInDegrees(is_focused);

        for (int i = 0; i < current_orb_count; ++i) {
            if (i >= degrees.size()) {
                break;
            }

            float angle_rad = degrees[i] * _PI / 180.0f;
            float offset_x = std::cos(angle_rad) * target_radius;
            float offset_y = -std::sin(angle_rad) * target_radius;

            Engine::Maths::Vec2 new_target_pos = { player_pos.x + offset_x, player_pos.y + offset_y};

            auto& state = orb_states[i];

            state.target_pos = new_target_pos;
            if (!state.is_transitioning) {
                bool mode_changed = (is_focused != state.last_focus_mode);

                if (mode_changed) {
                    state.current_pos = orbs[i]->GetComponent<TransformComponent>()->GetPosition();
                    state.transition_timer = 0.0f;
                    state.is_transitioning = true;
                }
            }
        }
    }

    void Reimu::StartEffect()
    {
        is_skilled = true;
        effect->GetComponent<SpriteComponent>()->SetHidden(false);
        effect->GetComponent<TransformComponent>()->SetPosition(role->GetComponent<TransformComponent>()->GetPosition());
        au.PlaySFX("se_power1", "skill");
    }


    void Reimu::InitCheckBox() {
        using namespace Engine::Core::Components;
        check_box = std::make_unique<Engine::Core::GameObject>();
        check_box->AddComponent<TransformComponent>(Engine::Maths::Vec2{ 0.0f, 0.0f }, Engine::Maths::Vec2{ 2.0f, 2.0f });
        check_box->AddComponent<SpriteComponent>(
            "eff_sloweffect",
            ctx.GetResourceManager(),
            SDL_FRect{ 0.0f, 0.0f, 64.0f, 64.0f },
            Align::CENTER
        );

        check_box->GetComponent<SpriteComponent>()->SetHidden(true);
    }

    void Reimu::InitOrbs() {
        using namespace Engine::Core::Components;

        for (int i = 0; i < 4; ++i) {
            auto orb = std::make_unique<Engine::Core::GameObject>();
            orb->AddComponent<TransformComponent>(Engine::Maths::Vec2{ 0.0f, 0.0f }, Engine::Maths::Vec2{ 2.0f, 2.0f });
            orb->AddComponent<SpriteComponent>(
                "pl00",
                ctx.GetResourceManager(),
                SDL_FRect{ 64.0f, 144.0f, 16.0f, 16.0f },
                Align::CENTER);

            orbs.push_back(std::move(orb));
            orb_states.push_back({});
            orb_states.back().current_pos = { 0.0f, 0.0f };
            orb_states.back().target_pos = { 0.0f, 0.0f };
        }
    }

    void Reimu::InitEffect()
    {
        effect = std::make_unique<Engine::Core::GameObject>();
        effect->AddComponent<TransformComponent>(Engine::Maths::Vec2{ 0.0f, 0.0f }, Engine::Maths::Vec2{ 0.5f, 0.5f });
        effect->AddComponent<SpriteComponent>(
            "eff_magicsquare",
            ctx.GetResourceManager(),
            SDL_FRect{ 0.0f, 0.0f, 256.0f, 256.0f },
            Align::CENTER
        );
        effect->GetComponent<SpriteComponent>()->SetHidden(true);
    }


    void Reimu::Update(f32 d_t) {
        using namespace Engine::Core::Components;
        Game::Entity::Player::Update(d_t);
        if (!role) return;

        if (shoot_cooldown_frames > 0) {
            --shoot_cooldown_frames;
        }
        auto player_comp = role->GetComponent<PlayerComponent>();
        bool is_focused = player_comp->GetIsSlow();
        if (current_orb_count != static_cast<i32>(gamedata.power)) {
            i32 count = static_cast<i32>(gamedata.power);
            SetOrb(count < 1 ? 1 : count);
        }

        
        SetOrbTargetPositions(is_focused);
        if (is_focused) {
            auto sp = check_box->GetComponent<SpriteComponent>();
            sp->SetHidden(false);
            check_box_rotation += 1.0f;
            if (check_box_rotation > 360.0f) {
                check_box_rotation = 0.0f;
            }
            sp->SetRotation(check_box_rotation);
        }
        else {
            check_box->GetComponent<SpriteComponent>()->SetHidden(true);
            check_box_rotation = 0.0f;
        }

        if (is_skilled) {
            skill_time_count += d_t;
            skill_rotation += 10.0f;
            if (skill_rotation > 360.0f) {
                skill_rotation = 0.0f;
            }
            effect_size += 0.5f;
            auto sp = effect->GetComponent<SpriteComponent>();
            sp->SetScale({ effect_size, effect_size });
            sp->SetRotation(skill_rotation);
            if (skill_time_count > skill_time) {
                skill_time_count = 0.0f;
                effect_size = 0.5f;
                skill_rotation = 0.0f;
                sp->SetHidden(true);
                is_muteki = false;
                is_skilled = false;
            }
        }
        effect->Update(d_t, ctx);

        if (check_box) {
            check_box->GetComponent<TransformComponent>()->SetPosition(role->GetComponent<TransformComponent>()->GetPosition());
            check_box->GetComponent<SpriteComponent>()->SetHidden(!is_focused);
            check_box->Update(d_t, ctx);
        }

        orbit_timer += d_t * ORB_ROTATION_SPEED;

        for (int i = 0; i < 4; ++i) {
            auto& state = orb_states[i];
            auto* orb_sprite = orbs[i]->GetComponent<SpriteComponent>();
            auto* orb_transform = orbs[i]->GetComponent<TransformComponent>();

            if (state.is_transitioning) {
                state.transition_timer += d_t;
                float t = state.transition_timer / state.TRANSITION_DURATION;

                if (t >= 1.0f) {
                    t = 1.0f;
                    state.is_transitioning = false;
                    state.transition_timer = state.TRANSITION_DURATION;

                    orb_transform->SetPosition(state.target_pos);
                    state.current_pos = state.target_pos;
                    state.last_focus_mode = is_focused;

                }
                else {
                    float current_x = state.current_pos.x + (state.target_pos.x - state.current_pos.x) * t;
                    float current_y = state.current_pos.y + (state.target_pos.y - state.current_pos.y) * t;

                    orb_transform->SetPosition({ current_x, current_y });
                }
            }

            if (!state.is_transitioning) {
                orb_transform->SetPosition(state.target_pos);
                state.current_pos = state.target_pos;
                state.last_focus_mode = is_focused;
            }
            if (orb_sprite) {
                orb_sprite->SetRotation(orbit_timer * 180.0f / _PI);
            }

            orbs[i]->Update(d_t, ctx);
        }
    }

    void Reimu::Render() {
        Player::Render();
        for (int i = 0; i < current_orb_count; ++i) {
            orbs[i]->Render(ctx);
        }
        if (check_box) {
            check_box->Render(ctx);
        }
        if (effect) {
            effect->Render(ctx);
        }
    }

    void Reimu::HandleInput() {
        Player::HandleInput();
        auto& input = ctx.GetInputManager();
        if (input.IsActionDown("ok") && shoot_cooldown_frames <= 0) {
            Shoot();
            shoot_cooldown_frames = reimu_base_fire_rate_frames;
        }
        if (input.IsActionPressed("skill") && !is_skilled && gamedata.power >= 1.0f) {
            StartEffect();
            bm.RemoveAllBullets();
            gamedata.power -= 1.0f;
            gamedata.score += 1000;
            if (gamedata.power < 0.0f) {
                gamedata.power = 0.0f;
            }
            is_muteki = true;
            auto& enemies = em.GetEnemies();
            for (const auto& e : enemies) {
                auto ec = e->GetEnemyComponent();
                if (ec->GetEnemyData().is_active) {
                    ec->Damage(1000);
                }
            }
        }
    }
}