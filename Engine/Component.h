#pragma once
namespace Engine::Core {
	class Context;
	class GameObject;

	class Component {
		friend class GameObject;

	protected:
		GameObject* owner = nullptr;

	public:
		Component() = default;
		virtual ~Component() = default;
		Component(const Component&) = delete;
		Component& operator=(const Component&) = delete;
		Component(Component&&) = delete;
		Component& operator=(Component&&) = delete;

	public:
		[[nodiscard]] GameObject* GetOwner() const { return owner; }
		void SetOwner(GameObject* o) { owner = o; }

	protected:
		virtual void Init() {};
		virtual void Render(Context&) {};
		virtual void Update(float, Context&) = 0;
		virtual void HandleInput(Context&) {};
		virtual void Destory() {};
	};
};