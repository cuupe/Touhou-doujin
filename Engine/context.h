#pragma once
namespace Engine::Resource {
	class ResourceMannager;
}
namespace Engine::Render {
	class Renderer;
}
namespace Engine::Core {
	class Context final {
	private:
		Render::Renderer& renderer;
		Resource::ResourceMannager& res;

	public:
		Context(Render::Renderer& RENDERER, Resource::ResourceMannager& RES);
		~Context() = default;
		Context(const Context&) = delete;
		Context& operator=(const Context&) = delete;
		Context(Context&&) = delete;
		Context& operator=(Context&&) = delete;


	public:
		Render::Renderer& GetRenderer() const { return renderer; }
		Resource::ResourceMannager& GetResourceMannager() const { return res; }
	};
}
