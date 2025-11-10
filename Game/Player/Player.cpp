#include "Player.h"
#include "../Engine/GameObject.h"
#include "../Engine/Components/TransformComponent.h"
#include "../Engine/Components/SpriteComponent.h"
#include "../Engine/Components/ColliderComponent.h"
#include "../Engine/Components/AnimationComponent.h"
#include "../Engine/Components/PlayerComponent.h"
namespace Game::GameObject {
	Player::Player(Engine::Core::Context& _ctx, const std::string& _name)
        :ctx(_ctx)
	{
		using namespace Engine::Core::Components;
		role = std::make_unique<Engine::Core::GameObject>("", "player");
		role->AddComponent<TransformComponent>(Vec2{ 512.0f, 650.0f }, Vec2{ 2.0f, 2.0f });
		role->AddComponent<SpriteComponent>("pl00", 
            ctx.GetResourceMannager(), SDL_FRect{ 0.0f, 0.0f, 256.0f / 8.0f, 48.0f },
            Align::CENTER);
        role->AddComponent<ColliderComponent>(
            std::make_unique<Engine::Core::Collider::AABBCollider>(
                Vec2{ 7.0f, 7.0f }), Align::CENTER);
        Animation anim_0("idle");
        for (float i = 0; i < 5; i += 1) {
            anim_0.AddFrame(SDL_FRect{ i * 32.0f, 0.0f, 32.0f, 48.0f }, 0.2);
        }
        Animation anim_1("left", false);
        anim_1.AddFrame(SDL_FRect{ 32.0f, 48.0f, 32.0f, 48.0f }, 0.05f);
        anim_1.AddFrame(SDL_FRect{ 2 * 32.0f, 48.0f, 32.0f, 48.0f }, 0.05f);
        anim_1.AddFrame(SDL_FRect{ 3 * 32.0f, 48.0f, 32.0f, 48.0f }, 0.05f);
        anim_1.AddFrame(SDL_FRect{ 4 * 32.0f, 48.0f, 32.0f, 48.0f }, 0.1f);
        anim_1.AddFrame(SDL_FRect{ 5 * 32.0f, 48.0f, 32.0f, 48.0f }, 0.2f);
        Animation anim_2("right", false);
        anim_2.AddFrame(SDL_FRect{ 32.0f, 96.0f, 32.0f, 48.0f }, 0.05f);
        anim_2.AddFrame(SDL_FRect{ 2 * 32.0f, 96.0f, 32.0f, 48.0f }, 0.05f);
        anim_2.AddFrame(SDL_FRect{ 3 * 32.0f, 96.0f, 32.0f, 48.0f }, 0.05f);
        anim_2.AddFrame(SDL_FRect{ 4 * 32.0f, 96.0f, 32.0f, 48.0f }, 0.1f);
        anim_2.AddFrame(SDL_FRect{ 5 * 32.0f, 96.0f, 32.0f, 48.0f }, 0.2f);
        role->AddComponent<AnimationComponent>();
        role->GetComponent<Engine::Core::Components::AnimationComponent>()->AddAnimation(
            std::make_unique<Animation>(anim_0));
        role->GetComponent<Engine::Core::Components::AnimationComponent>()->AddAnimation(
            std::make_unique<Animation>(anim_1));
        role->GetComponent<Engine::Core::Components::AnimationComponent>()->AddAnimation(
            std::make_unique<Animation>(anim_2));
        role->GetComponent<Engine::Core::Components::AnimationComponent>()->PlayAnimaiton("idle");
        role->AddComponent<Engine::Core::Components::PlayerComponent>(500.0f);

		check = std::make_unique<Engine::Core::GameObject>("", "player_check");
		check->AddComponent<TransformComponent>(Vec2{ 0.0f, 0.0f }, Vec2{ 2.0f, 2.0f });
        //check->AddComponent<SpriteComponent>("eff_sloweffect", 
        //    ctx.GetResourceMannager(), SDL_FRect{});
		check->AddComponent<ColliderComponent>(std::make_unique<Engine::Core::Collider::AABBCollider>(
            Vec2{ 20.0f, 20.0f }), Align::CENTER);
	}

	f32 Player::AngleToPlayer(const Vec2& pos)
	{
		auto& this_pos = role->GetComponent<Engine::Core::Components::TransformComponent>()->GetPosition();
		Vec2 dir = this_pos - pos;

		f32 angle = Engine::Maths::Vec2ToAngle(dir);
		return angle;
	}

    void Player::SetToNormal()
    {
        role->GetComponent<TransformComponent>()->SetPosition({512.0f, 700.0f});
    }

    void Player::Update(f32 d_t) {
        role->Update(d_t, ctx);
        check->GetComponent<TransformComponent>()->SetPosition(
            role->GetComponent<TransformComponent>()->GetPosition());
        check->Update(d_t, ctx);
    }

    void Player::HandleInput() {
        role->HandleInput(ctx);
        check->HandleInput(ctx);
    }

    void Player::Render() {
        role->Render(ctx);
        check->Render(ctx);
        auto& pos = role->GetComponent<TransformComponent>()->GetPosition();
        SDL_FRect f = { pos.x - 7.0f, pos.y - 7.0f, 14.0f, 14.0f };
        SDL_RenderRect(ctx.GetRenderer().getSDLRenderer(), &f);
    }


}

namespace Game::GameObject::PlayerUtils {
	void Init_Player(const std::string& _name, float _speed)
	{

	}


}

