#pragma once
#include "../prefix.h"
#include "maths.h"
namespace Engine::Core::Collider {
	enum class ColliderType {
		NONE,
		AABB,
		CIRCLE
	};

	//本质上都是AABB的拓展，所以基类使用AABB
	class Collider {
	protected:
		Engine::Maths::Vec2 aabb_size = { 0.0f, 0.0f };

	public:
		virtual ~Collider() = default;
		virtual ColliderType GetType() const = 0;
		void Set_AABB_Size(Engine::Maths::Vec2 size) { aabb_size = std::move(size); }
		const Engine::Maths::Vec2& Get_AABB_Size() const { return aabb_size; }
	};

	class AABBCollider final : public Collider {
	private:
		Engine::Maths::Vec2 size = { 0.0f, 0.0f };

	public:
		explicit AABBCollider(Engine::Maths::Vec2 size) : size(size) { Set_AABB_Size(std::move(size)); }
		~AABBCollider() override = default;

		ColliderType GetType() const override { return ColliderType::AABB; }
		const Engine::Maths::Vec2& GetSize() const { return size; }
		void SetSize(Engine::Maths::Vec2& size_) { size = std::move(size_); }
	};

	class CircleCollider final : public Collider {
	private:
		float radius = 0.0f;

	public:
		explicit CircleCollider(float radius_) : radius(radius_) { Set_AABB_Size(Engine::Maths::Vec2(radius_ * 2.0f, radius_ * 2.0f)); }
		~CircleCollider() override = default;

		ColliderType GetType() const override { return ColliderType::CIRCLE; }
		float GetRadius() const { return radius; }
		void SetRadius(float radius_) { radius = radius_; }

	};
	

	bool CheckCollision(
		const ColliderComponent& a,
		const ColliderComponent& b);

	bool CheckCircleOverlap(
		const Engine::Maths::Vec2& a_center,
		float a_redius,
		const Engine::Maths::Vec2& b_center,
		float b_redius);

	bool CheckAABBOverlap(
		const Engine::Maths::Vec2& a_pos,
		const Engine::Maths::Vec2& a_size,
		const Engine::Maths::Vec2& b_pos,
		const Engine::Maths::Vec2& b_size);

	bool CheckRectOverlap(
		const Engine::Maths::Rect& a,
		const Engine::Maths::Rect& b);

	bool CheckPointInCircle(
		const Engine::Maths::Vec2& point,
		const Engine::Maths::Vec2& center,
		float radius);
}