#pragma once
namespace Engine {
	class Component {
		friend class GameObject;

	protected:
		GameObject* owner;

	public:
		Component() = default;
		virtual ~Component() = default;
		Component(const Component&) = delete;
		Component& operator=(const Component&) = delete;
		Component(Component&&) = delete;
		Component& operator=(Component&&) = delete;

	public:
		GameObject* GetOwner() const { return owner; }
		void SetOwner(GameObject* o) { owner = o; }

	protected:
		virtual void Init();
		virtual void Render();
		virtual void Update();
		virtual void HandleInput();
		virtual void Destory();
	};
}
