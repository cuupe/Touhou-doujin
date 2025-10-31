#pragma once
#include "../Component.h"
#include "../GameObject.h"
#include "../maths.h"
using namespace Engine::Maths;
namespace Engine::Core::Components {
	class TransformComponent final :public Component {
		friend class Engine::Core::GameObject;

	private:
		Vec2 position;
		Vec2 scale;
		float rotation;

	public:
		TransformComponent(
			Vec2 pos = { 0.0f, 0.0f },
			Vec2 sc = { 1.0f, 1.0f },
			float rotate = 0.0f);
		TransformComponent(const TransformComponent&) = delete;
		TransformComponent& operator=(const TransformComponent&) = delete;
		TransformComponent(TransformComponent&&) = delete;
		TransformComponent& operator=(TransformComponent&&) = delete;


	public:
		const Vec2& GetPosition() const;
		const Vec2& GetScale() const;
		float GetRotation() const;
		void SetPosition(const Vec2&);
		void SetPosition(float, float);
		void SetScale(const Vec2&);
		void SetScale(float, float);
		void SetRotation(float);
	};

}
