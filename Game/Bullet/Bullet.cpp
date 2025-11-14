#include "Bullet.h"
#include "../../Engine/GameObject.h"
#include "../../Engine/Components/TransformComponent.h"
#include "../../Engine/Components/SpriteComponent.h"
#include "../../Engine/Components/ColliderComponent.h"
#include "../../Engine/Components/AnimationComponent.h"
#include "../../Engine/collider.h"
#include "BulletComponent.h"
namespace Game::Bullets {
	Bullet::Bullet(BulletData& _bd,
		Engine::Core::Context& _ctx)
		:ctx(_ctx), bd(_bd)
	{
		core = std::make_unique<Engine::Core::GameObject>();
		core->AddComponent<Engine::Core::Components::TransformComponent>(
			bd.position, Vec2{2.0f, 2.0f});
		core->AddComponent<Engine::Core::Components::SpriteComponent>(
			bd.sprite_name, _ctx.GetResourceManager(), bd.rect,
			Engine::Maths::Align::CENTER);
		core->AddComponent<Engine::Core::Components::ColliderComponent>(
			std::make_unique<Engine::Core::Collider::AABBCollider>(bd.hit_size),
			Engine::Maths::Align::CENTER);
		core->AddComponent<Game::Component::BulletComponent>();
		graze = std::make_unique<Engine::Core::GameObject>();
		graze->AddComponent<Engine::Core::Components::TransformComponent>(
			bd.position);
		graze->AddComponent<Engine::Core::Components::ColliderComponent>(
			std::make_unique<Engine::Core::Collider::AABBCollider>(bd.graze_size * 0.5f),
			Engine::Maths::Align::CENTER);

	}

	void Bullet::Update(f32 d_t) {
		auto transform = core->GetComponent<Engine::Core::Components::TransformComponent>();
		auto sprite = core->GetComponent<Engine::Core::Components::SpriteComponent>();
		auto collider = core->GetComponent<Engine::Core::Components::ColliderComponent>();
		auto anim = core->GetComponent<Engine::Core::Components::AnimationComponent>();
		transform->Translate(bd.v);
		bd.position = transform->GetPosition();
		bd.angle += bd.angle_acc * d_t;
		if (bd.restrict_angle) {
			if (bd.angle > bd.max_angle) {
				bd.angle = bd.max_angle;
			}
		}
		transform->SetRotationDeg(bd.angle + 90.0f);
		bd.speed += bd.speed_acc * d_t;
		if (bd.restrict_speed) {
			if (bd.speed > bd.max_speed) {
				bd.speed = bd.max_speed;
			}
		}

		bd.v = { bd.speed * cos(Engine::Maths::DegToRad(bd.angle)), bd.speed * sin(Engine::Maths::DegToRad(bd.angle)) };

		if (bd.position.x > 1500.0f || bd.position.x < -200.0f || bd.position.y > 1000.0f || bd.position.y < -100.0f) {
			bd.is_active = false;
			bd.is_grazed = false;
		}

		core->Update(d_t, ctx);
	}

	void Bullet::Render() {
		core->Render(ctx);
		auto& render = ctx.GetRenderer();
		SDL_FRect f{ bd.position.x - bd.graze_size.x / 2.0f, bd.position.y - bd.graze_size.y / 2.0f,
			bd.graze_size.x, bd.graze_size.y };
		SDL_RenderRect(render.getSDLRenderer(), &f);
	}
}