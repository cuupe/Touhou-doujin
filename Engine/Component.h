#pragma once
namespace Engine::Core {
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
		virtual void Render() {};
		virtual void Update(float) {};
		virtual void HandleInput() {};
		virtual void Destory() {};
	};
};