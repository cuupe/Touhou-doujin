#pragma once
#include "../prefix.h"
#include "time.h"
using Microsoft::WRL::ComPtr;
namespace Engine {
	class engine {
	protected:
		SDL_Window* window = nullptr;
		SDL_Renderer* renderer = nullptr;
		MIX_Mixer* mixer = nullptr;
		ComPtr<ID3D11Device> device;
		ComPtr<ID3D11DeviceContext> context;
		bool initialized = false;


	protected:
		int width, height;
		time t;
		SDL_Event event;

	public:
		engine(const char* win_name,
			int width, int height, int flag,
			int fps);
		engine(const engine&) = delete;
		engine(engine&&) = delete;
		engine& operator=(const engine&) = delete;
		engine& operator=(engine&&) = delete;
		virtual ~engine();
		
	public:
		SDL_Window* GetWindow() const { return window; }
		SDL_Renderer* GetRenderer() const { return renderer; }
		ID3D11Device* GetDevice() const { return device.Get(); }
		ID3D11DeviceContext* GetDeviceContext() const { return context.Get(); }
		bool GetInitState() const { return initialized; }

	protected:
		virtual void HandleInput() = 0;
		virtual void Render() = 0;
		virtual void Update() = 0;


	public:
		virtual void Run() = 0;
	};
}
