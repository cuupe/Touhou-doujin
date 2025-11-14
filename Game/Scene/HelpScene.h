#pragma once
#include "../../Engine/scene.h"
#include "../GameData.h"
namespace Engine::Audio {
	class AudioManager;
}

//namespace Game::Scene {
//	class HelpScene final :public Engine::Scene::Scene {
//	private:
//		std::vector<std::unique_ptr<Engine::Core::GameObject>> opts;
//		std::vector<std::unique_ptr<Engine::Core::GameObject>> bcgs;
//		i16 index;
//
//	public:
//		HelpScene(const std::string& name, Engine::Core::Context& ctx, Engine::Scene::SceneManager& s_m,
//			Engine::Audio::AudioManager& au);
//
//
//	public:
//		void Init() override;
//		void HandleInput() override;
//		void Update(float) override;
//		void Render() override;
//	};
//}