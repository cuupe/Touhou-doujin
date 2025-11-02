#pragma once
#include "../prefix.h"
#include "../Engine/engine.h"
#include "../Engine/render.h"
#include "../Engine/time.h"
#include "../Engine/resources.h"
#include "../Engine/Component.h"
#include "../Engine/GameObject.h"
#include "../Engine/maths.h"
#include "../Engine/context.h"
#include "../Engine/scene.h"
#include "../Engine/SceneManager.h"

namespace Game {
	class Game final : public Engine::engine {
	private:
		bool running = true;
		bool initialized = false;

	private:
		//这里负责统一流程的管理，不属于资源
		Engine::Resource::TrackPtr bgm;	//背景音乐
		std::map<std::string, Engine::Resource::TrackPtr> sfx;	//音频轨道 - 音效

	private:
		std::unique_ptr<Engine::Core::Context> ctx;
		std::unique_ptr<Engine::Resource::ResourceMannager> res;
		std::unique_ptr<Engine::Render::Renderer> r;
		std::unique_ptr<Engine::Scene::SceneManager> sc;

	public:
		Game(const char* win_name, int width, int height, int flag, int fps);
		Game(const Game&) = delete;
		Game(Game&&) = delete;
		~Game();

	private:
		void HandleInput() override;
		void Render() override;
		void Update() override;

	public:
		void Run() override;

	public:
		bool GetInit() const { return initialized; }

	public:
		//test
		void test();
	};
}