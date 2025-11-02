#include "../prefix.h"
#include "maths.h"
using namespace Engine::Maths;
namespace Engine::Core::Collider {
	enum class ColliderType {
		NONE,
		AABB,
		CIRCLE
	};
	//本质上都是AABB的拓展，所以基类使用AABB
	class Collider {
	protected:
		Vec2 aabb_size = { 0.0f, 0.0f };

	public:
		virtual ~Collider() = default;
		virtual ColliderType GetType() const = 0;
		void Set_AABB_Size(const Vec2& size) { aabb_size = std::move(size); }
		const Vec2& Get_AABB_Size() const { return aabb_size; }
	};

	class AABBCollider final : public Collider {
	private:
		Vec2 size = { 0.0f, 0.0f };

	public:
		explicit AABBCollider(const Vec2& size) : size(std::move(size)) { Set_AABB_Size(size); }
		~AABBCollider() override = default;

		ColliderType GetType() const override { return ColliderType::AABB; }
		const Vec2& GetSize() const { return size; }
		void SetSize(const Vec2& size_) { size = std::move(size_); }
	};

	class CircleCollider final : public Collider {
	private:
		float radius = 0.0f;

	public:
		explicit CircleCollider(float radius_) : radius(radius_) { Set_AABB_Size(Vec2(radius_ * 2.0f, radius_ * 2.0f)); }
		~CircleCollider() override = default;

		ColliderType GetType() const override { return ColliderType::CIRCLE; }
		float GetRadius() const { return radius; }
		void SetRadius(float radius_) { radius = radius_; }

	};
}