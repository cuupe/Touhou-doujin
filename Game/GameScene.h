#pragma once
#include "../prefix.h"
#include "../Engine/context.h"
#include "../Engine/GameObject.h"
#include "../Engine/scene.h"
#include "../Engine/Components/TransformComponent.h"
#include "../Engine/Components/SpriteComponent.h"

namespace Game::Scene {
	class GameScene final : public Engine::Scene::Scene {
	public:
		GameScene(const std::string& name,
			Engine::Core::Context& ctx,
			Engine::Scene::SceneManager& s_m);
	
	public:
		void Init() override;
		void Update(float) override;
		void Render() override;
		void HandleInput() override;
		void Destroy() override;

	private:
		void test();
	};
}