#include "../prefix.h"
#include "context.h"
namespace Engine::Core {
	Context::Context(Render::Renderer& RENDERER, Resource::ResourceManager& RES,
		Input::InputManager& INPUT, Time::Schedule& SCH, ID3D11Device& _device,
		ID3D11DeviceContext& _device_context)
		:renderer(RENDERER), res(RES), input(INPUT), schedule(SCH), device(&_device),
		device_context(&_device_context)
	{ }
}