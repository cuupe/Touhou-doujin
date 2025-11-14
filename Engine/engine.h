#pragma once
#include "../prefix.h"
#include "time.h"
namespace Engine {
	using Microsoft::WRL::ComPtr;
	class engine {
	protected:
		SDL_Window* window = nullptr;
		SDL_Renderer* renderer = nullptr;
		ComPtr<ID3D11Device> device;
		ComPtr<ID3D11DeviceContext> device_context;
		bool initialized = false;

	protected:
		Time::time t;
		SDL_Event event;

	public:
		engine(const char* win_name, int width,
			int height, int flag, int fps);
		engine(const engine&) = delete;
		engine(engine&&) = delete;
		engine& operator=(const engine&) = delete;
		engine& operator=(engine&&) = delete;
		virtual ~engine();
		
	public:
		SDL_Window* GetWindow() const { return window; }
		SDL_Renderer* GetRenderer() const { return renderer; }
		ID3D11Device* GetDevice() const { return device.Get(); }
		ID3D11DeviceContext* GetDeviceContext() const { return device_context.Get(); }
		bool GetInitState() const { return initialized; }

	protected:
		virtual void HandleInput() = 0;
		virtual void Render() = 0;
		virtual void Update() = 0;


	public:
		virtual void Run() = 0;
	};
}
