#include "../prefix.h"
#include "context.h"
namespace Engine::Core {
	Context::Context(Render::Renderer& RENDERER, Resource::ResourceMannager& RES,
		Input::InputManager& INPUT)
		:renderer(RENDERER), res(RES), input(INPUT)
	{ }
}