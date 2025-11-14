#include "../prefix.h"
#include "context.h"
namespace Engine::Core {
	Context::Context(Render::Renderer& RENDERER, Resource::ResourceManager& RES,
		Input::InputManager& INPUT, Time::Schedule& SCH)
		:renderer(RENDERER), res(RES), input(INPUT), schedule(SCH)
	{ }
}