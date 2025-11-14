#pragma once
#include "../Engine/GameObject.h"
#include "../Bullet/Bullet.h"
#include "../../Engine/context.h"
namespace Game::GameObject {
	class Player {
	private:
		std::unique_ptr<Engine::Core::GameObject> role;
		std::unique_ptr<Engine::Core::GameObject> check;
		Engine::Core::Context& ctx;

	public:
		Player(Engine::Core::Context&, const std::string& name = "");
		~Player() = default;

	public:
		f32 AngleToPlayer(const Engine::Maths::Vec2& pos);

		void SetToNormal();

	public:
		Engine::Core::GameObject* GetPlayer() const { return role.get(); }
		Engine::Core::GameObject* GetPlayerCheckBox() const { return check.get(); }

	public:
		void Update(f32 d_t);
		void HandleInput();
		void Render();
	};
}

