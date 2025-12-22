#pragma once
#include "../../Engine/scene.h"
#include "../../Engine/UIManager.h"
#include "../GameData.h"
#include <mutex>
#include <thread>
#include <atomic>
#include <vector>
#include <queue>
namespace Engine::UISystem {
	class UIManager;
}
namespace Engine::Render {
	class Renderer;
}
namespace Game::Scene {
	using Microsoft::WRL::ComPtr;
	class Loading final :public Engine::Scene::Scene {
	private:
		struct ResourceLoadInfo {
			std::string type;
			std::string path;
		};

		std::queue<ResourceLoadInfo> tasks;
		mutable std::mutex tasks_mutex;
		std::atomic<bool> request_stop{ false };
		std::atomic<u32> total_tasks{ 0 };
		std::atomic<u32> completed_tasks{ 0 };
		std::thread loading_thread;
	private:
		bool on_leaving = false;
		u8 alpha = 255;

	public:
		Loading(const std::string&, Engine::Core::Context&, Engine::Scene::SceneManager&,
			Engine::Audio::AudioManager&, Engine::UISystem::UIManager&, Engine::Render::TextManager&);


	public:
		void Init() override;
		void HandleInput() override { };
		void Update(float) override;
		void Render() override;
	};
}