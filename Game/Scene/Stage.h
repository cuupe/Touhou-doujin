#pragma once
#include "../../Engine/scene.h"
#include "../Bullet/BulletManager.h"
#include <wrl/client.h>
using Microsoft::WRL::ComPtr;
namespace Game::GameObject {
	class Player;
}
namespace Engine::Audio {
	class AudioManager;
}
namespace Engine::UISystem {
	class UIManager;
}
namespace Game::Scene {
	class Stage final :public Engine::Scene::Scene {
	private:
		std::unique_ptr<Game::Manager::BulletManager> bm;
		std::unique_ptr<Game::GameObject::Player> player;
		Engine::Audio::AudioManager& au;
	private:
		bool is_pause = false;

	public:
		Stage(const std::string& name,
			Engine::Core::Context& ctx,
			Engine::Scene::SceneManager& s_m,
			Engine::Audio::AudioManager& _au);

	public:
		void Init() override;
		void HandleInput() override;
		void Update(float) override;
		void Render() override;
	};
}