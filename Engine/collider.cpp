#include "collider.h"
#include "Components/ColliderComponent.h"
namespace Engine::Core::Collider {
	bool CheckCollision(const ColliderComponent& a, const ColliderComponent& b)
	{
		auto a_collider = a.GetCollider();
		auto b_collider = b.GetCollider();
		auto a_trans = a.GetTransform();
		auto b_trans = b.GetTransform();

		//粗略计算最小包围圈的碰撞
		auto a_size = a_collider->Get_AABB_Size() * a_trans->GetScale();
		auto b_size = b_collider->Get_AABB_Size() * b_trans->GetScale();
		auto a_pos = a_trans->GetPosition() + a.GetOffset();
		auto b_pos = b_trans->GetPosition() + b.GetOffset();


		if (!CheckAABBOverlap(a_pos, a_size, b_pos, b_size)) {
			return false;
		}

		if (a_collider->GetType() == ColliderType::AABB &&
			b_collider->GetType() == ColliderType::AABB) {
			return true;
		}
		else if (a_collider->GetType() == ColliderType::CIRCLE &&
			b_collider->GetType() == ColliderType::CIRCLE) {
			auto a_center = a_pos + a_size * 0.5f;
			auto b_center = b_pos + b_size * 0.5f;
			auto a_radius = 0.5f * a_size.x;
			auto b_radius = 0.5f * b_size.x;
			return CheckCircleOverlap(a_center, a_radius, b_center, b_radius);
		}
		else if (a_collider->GetType() == ColliderType::AABB &&
			b_collider->GetType() == ColliderType::CIRCLE) {
			auto b_center = b_pos + 0.5f * b_size;
			auto b_radius = 0.5f * b_size.x;
			auto nearest_point = Clamp(b_center, b_pos, b_pos + b_size);
			return CheckPointInCircle(nearest_point, b_center, b_radius);
		}
		else if (a_collider->GetType() == ColliderType::CIRCLE &&
			b_collider->GetType() == ColliderType::AABB) {
			auto a_center = a_pos + 0.5f * a_size;
			auto a_radius = 0.5f * a_size.x;
			auto nearest_point = Clamp(a_center, a_pos, a_pos + a_size);
			return CheckPointInCircle(nearest_point, a_center, a_radius);
		}
		return false;
	}

	//检查圆的重叠，直接检查两圆圆心的距离是否大于两圆半径的和
	bool CheckCircleOverlap(const Engine::Maths::Vec2& a_center, float a_radius, 
		const Engine::Maths::Vec2& b_center, float b_radius)
	{
		return (Engine::Maths::Vec2_Length(a_center - b_center) < a_radius + b_radius);
	}

	//检查矩形的重叠直接检测四个顶点是否处于另一个矩形内（基于分离轴定理）
	bool CheckAABBOverlap(const Engine::Maths::Vec2& a_pos, const Engine::Maths::Vec2& a_size, 
		const Engine::Maths::Vec2& b_pos, const Engine::Maths::Vec2& b_size)
	{
		if (a_pos.x + a_size.x <= b_pos.x || a_pos.x >= b_pos.x + b_size.x ||
			a_pos.y + a_size.y <= b_pos.y || a_pos.y >= b_pos.y + b_size.y) {
			return false;
		}
		return true;
	}

	bool CheckRectOverlap(const Engine::Maths::Rect& a, const Engine::Maths::Rect& b)
	{
		return CheckAABBOverlap(Engine::Maths::Vec2{ a.x, a.y }, Engine::Maths::Vec2{ a.w, a.h },
			Engine::Maths::Vec2{ b.x, b.y }, Engine::Maths::Vec2{ b.w, b.h });
	}

	bool CheckPointInCircle(const Engine::Maths::Vec2& point, const Engine::Maths::Vec2& center, float radius)
	{
		return (Engine::Maths::Vec2_Length(point - center) < radius);
	}
}