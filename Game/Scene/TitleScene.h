#pragma once
#include "../../Engine/scene.h"
#include "../GameData.h"
namespace Engine::Audio {
	class AudioManager;
}

namespace Game::Scene {
	class TitleScene final :public Engine::Scene::Scene {
	private:
		std::vector<std::unique_ptr<Engine::Core::GameObject>> opts;
		std::vector<std::unique_ptr<Engine::Core::GameObject>> bcgs;
		Engine::Audio::AudioManager& au;
		i16 index;
		i16 old_index;
		bool first;

	public:
		TitleScene(const std::string& name, Engine::Core::Context& ctx, Engine::Scene::SceneManager& s_m,
			Engine::Audio::AudioManager& au);


	public:
		void Init() override;
		void HandleInput() override;
		void Update(float) override;
		void Render() override;
	};
}
