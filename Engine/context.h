#pragma once
#include <d3d11.h>
namespace Engine::Resource {
	class ResourceManager;
}
namespace Engine::Render {
	class Renderer;
}
namespace Engine::Input {
	class InputManager;
}
namespace Engine::Time {
	class Schedule;
}
namespace Engine::Core {
	class Context final {
	private:
		Render::Renderer& renderer;
		Resource::ResourceManager& res;
		Input::InputManager& input;
		Time::Schedule& schedule;
		ID3D11Device* device;
		ID3D11DeviceContext* device_context;

	public:
		Context(Render::Renderer& RENDERER,
			Resource::ResourceManager& RES,
			Input::InputManager& INPUT,
			Time::Schedule& schedule,
			ID3D11Device& _device,
		ID3D11DeviceContext& _device_context);
		~Context() = default;
		Context(const Context&) = delete;
		Context& operator=(const Context&) = delete;
		Context(Context&&) = delete;
		Context& operator=(Context&&) = delete;


	public:
		Render::Renderer& GetRenderer() const { return renderer; }
		Resource::ResourceManager& GetResourceManager() const { return res; }
		Input::InputManager& GetInputManager() const { return input; }
		Time::Schedule& GetSchedule() const { return schedule; }
		ID3D11Device* GetDevice() const { return device; };
		ID3D11DeviceContext* GetDeviceContext() const { return device_context; }
	};
}
