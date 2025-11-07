#pragma once
#include "../../Engine/scene.h"
#include "../Bullet/BulletManager.h"
namespace Game::Scene {
	class Stage final :Engine::Scene::Scene {
	private:
		std::unique_ptr<Game::Manager::BulletManager> bm;


	public:
		Stage(const std::string& name,
			Engine::Core::Context& ctx,
			Engine::Scene::SceneManager& s_m);

	public:
		void Update(float) override;
		void Render() override;
	};
}