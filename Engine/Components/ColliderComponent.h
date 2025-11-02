#pragma once
#include "../prefix.h"
#include "../maths.h"
#include "../Component.h"
#include "TransformComponent.h"
#include "../collider.h"
using namespace Engine::Maths;
using namespace Engine::Core::Collider;
namespace Engine::Core::Components {
	class ColliderComponent final :public Component {
		friend class GameObject;

	private:
		TransformComponent* trans = nullptr;
		std::unique_ptr<Collider::Collider> collider;
		Vec2 offset = { 0.0f, 0.0f };  //坐标按照左上角变换
		Align align = Align::NONE;

		bool is_trigger = false;
		bool is_active = true;

	public:
		explicit ColliderComponent(
			std::unique_ptr<Collider::Collider> collider_,
			Align alignment = Align::NONE,
			bool is_trigger_ = false,
			bool is_active_ = true
		);

		void UpdateOffset();

		const TransformComponent* GetTransform() const { return trans; }
		const Collider::Collider* GetCollider() const { return collider.get(); }
		const Vec2& GetOffset() const { return offset; }
		Align GetAlignment() const { return align; }
		Rect GetWorldAABB() const;
		bool IsTrigger() const { return is_trigger; }
		bool IsActive() const { return is_active; }
		void SetAlignment(Align al);
		void SetOffset(const Vec2& offset_) { offset = offset_; }
		void SetTrigger(bool i_t) { is_trigger = i_t; }
		void SetActive(bool i_a) { is_active = i_a; }

	private:
		void Init() override;
		void Update(float, Context&) override {}

	};
	

}