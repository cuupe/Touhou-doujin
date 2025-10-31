#pragma once
#include "../GameObject.h"
#include "../resources.h"
#include "../Component.h"
#include "../maths.h"
#include "../render.h"
#include "TransformComponent.h"
using namespace Engine::Maths;
using namespace Engine::Resource;
using namespace Engine::Render;
namespace Engine::Core::Components {
	class SpriteComponent final :public Component {
		friend class Engine::Core::GameObject;
	private:
		ResourceMannager* res = nullptr;
		TransformComponent* trans = nullptr;

		Sprite s;
		Align align;
		SDL_FRect rect;
		bool is_filpped = false;

	public:
		//SpriteComponent(
		//	const std::string& t_name,
		//	ResourceMannager& r,
		//	Align a,

		//)
	};
}