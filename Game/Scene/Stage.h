#pragma once
#include "../../Engine/scene.h"
#include "../Bullet/BulletManager.h"
namespace Game::GameObject {
	class Player;
}
namespace Engine::Audio {
	class AudioManager;
}
namespace Game::Scene {
	class Stage final :public Engine::Scene::Scene {
	private:
		std::unique_ptr<Game::Manager::BulletManager> bm;
		std::unique_ptr<Game::GameObject::Player> player;

	private:
		bool is_pause = false;

	public:
		Stage(const std::string& name,
			Engine::Core::Context& ctx,
			Engine::Scene::SceneManager& s_m,
			Engine::Audio::AudioManager& au);

	public:
		void Init() override;
		void HandleInput() override;
		void Update(float) override;
		void Render() override;
	};
}