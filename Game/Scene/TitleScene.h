#pragma once
#include "../../Engine/scene.h"
#include "../../Engine/UIManager.h"
#include "../GameData.h"
namespace Engine::Audio {
	class AudioManager;
}
namespace Engine::UISystem {
	class UIManager;
}
namespace Game::Scene {
	class TitleScene final :public Engine::Scene::Scene {
	private:
		Data data;

	public:
		TitleScene(const std::string&, Engine::Core::Context&, Engine::Scene::SceneManager&,
			Engine::Audio::AudioManager&, Engine::UISystem::UIManager&);

	private:
		void CreateBackgrounds();

	public:
		void Init() override;
		void HandleInput() override;
		void Update(float) override;
		void Render() override;
	};
}
