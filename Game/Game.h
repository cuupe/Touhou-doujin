#pragma once
#include "../prefix.h"
#include "../Engine/engine.h"
#include "../Engine/time.h"
#include "resources.h"
using namespace Resources;
class Game : public Engine::engine {
private:
	bool running = true;
	bool initialized = false;

private:
	TrackPtr bgm;	//背景音乐
	std::map<std::string, TrackPtr> sfx;	//音频轨道 - 音效

	std::map<std::string, std::unique_ptr<AudioResource>> audios;
	std::map<std::string, std::unique_ptr<TextureResource>> texs;
	std::map<std::string, std::unique_ptr<FontResource>> fonts;


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
};