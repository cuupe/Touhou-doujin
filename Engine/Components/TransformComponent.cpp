#include "TransformComponent.h"
namespace Engine::Core::Components {
	TransformComponent::TransformComponent(Vec2 pos, Vec2 sc, float rotate)
		:position(pos), scale(sc), rotation(rotate) { }

	const Vec2& TransformComponent::GetPosition() const
	{
		return position;
	}

	const Vec2& TransformComponent::GetScale() const
	{
		return scale;
	}

	float TransformComponent::GetRotation() const
	{
		return rotation;
	}

	void TransformComponent::SetPosition(const Vec2& pos)
	{
		position = pos;
	}

	void TransformComponent::SetPosition(float x, float y)
	{
		position.x = x;
		position.y = y;
	}

	void TransformComponent::SetScale(const Vec2& sc)
	{
		scale = sc;
	}

	void TransformComponent::SetScale(float x, float y)
	{
		scale.x = x;
		scale.y = y;
	}

	void TransformComponent::SetRotationDeg(float rotate)
	{
		rotation = fmod(rotate, 360.0f);
	}

	void TransformComponent::SetRotationRad(float angle)
	{
		rotation = fmod(RadToDeg(angle), 360.0f);
	}

	void TransformComponent::Translate(const Vec2& vec)
	{
		position += vec;
	}

}