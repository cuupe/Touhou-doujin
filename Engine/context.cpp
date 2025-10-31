#include "../prefix.h"
#include "context.h"
namespace Engine::Core {
	Context::Context(Render::Renderer& RENDERER, Resource::ResourceMannager& RES)
		:renderer(RENDERER), res(RES)
	{
		spdlog::trace("上下文已创建并初始化");
	}
}