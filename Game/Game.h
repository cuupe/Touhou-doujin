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
#include "../Engine/InputManager.h"
#include "../Engine/AudioManager.h"
#include "../Engine/config.h"
namespace Game {
	class Game final : public Engine::engine {
	private:
		bool running = true;
		bool initialized = false;

	private:
		//std::unique_ptr<Engine::Core::Config> config;
		std::unique_ptr<Engine::Core::Context> ctx;
		std::unique_ptr<Engine::Audio::AudioManager> audio;
		std::unique_ptr<Engine::Resource::ResourceMannager> res;
		std::unique_ptr<Engine::Scene::SceneManager> sc;
		std::unique_ptr<Engine::Input::InputManager> input;
		std::unique_ptr<Engine::Render::Renderer> r;

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