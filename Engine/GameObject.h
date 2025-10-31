#pragma once
#include "../prefix.h"
#include "Component.h"
namespace Engine::Core {
	class GameObject final{
	protected:
		std::string name;
		std::string tag;
		std::unordered_map<std::type_index, std::unique_ptr<Component>> components;
		bool need_remove = false;

	public:
		GameObject(const std::string& name = "", const std::string& tag = "");
		virtual ~GameObject() = default;
		GameObject(const GameObject&) = delete;
		GameObject& operator=(const GameObject&) = delete;
		GameObject(GameObject&&) = delete;
		GameObject& operator=(GameObject&&) = delete;

	public:
		void SetName(const std::string& new_name) { name = new_name; }
		const std::string& GetName() const { return name; }
		void SetTag(const std::string& new_tag) { tag = new_tag; }
		const std::string& GetTag() const { return tag; }
		void SetNeedRemove(bool new_state) { need_remove = new_state; }
		const bool GetNeedRemove() const { return need_remove; }


		template <typename T, typename... Args>
		T* AddComponent(Args&&... args) {
			static_assert(std::is_base_of<Component, T>::value, "T 必须继承自 Component");
			// 获取类型标识。     /* typeid(T) -- 用于获取一个表达式或类型的运行时类型信息 (RTTI), 返回 std::type_info& */
								/* std::type_index -- 针对std::type_info对象的包装器，主要设计用来作为关联容器（如 std::map）的键。*/
			auto type_index = std::type_index(typeid(T));
			// 如果组件已经存在，则直接返回组件指针
			if (HasComponent<T>()) {
				return GetComponent<T>();
			}
			// 如果不存在则创建组件     /* std::forward -- 用于实现完美转发。传递多个参数的时候使用...标识 */
			auto new_component = std::make_unique<T>(std::forward<Args>(args)...);
			T* ptr = new_component.get();
			new_component->SetOwner(this);
			components[type_index] = std::move(new_component);
			ptr->Init();
			spdlog::debug("GameObject::AddComponent: {} added component {}", name, typeid(T).name());
			return ptr;
		}

		//用于获取组件
		template <typename T>
		T* GetComponent() const {
			static_assert(std::is_base_of<Component, T>::value, "T 必须继承自 Component");
			auto type_index = std::type_index(typeid(T));
			auto it = components.find(type_index);
			if (it != components.end()) {
				return static_cast<T*>(it->second.get());
			}
			return nullptr;
		}

		//用于判断是否含有组件
		template <typename T>
		bool HasComponent() const {
			static_assert(std::is_base_of<Component, T>::value, "T 必须继承自 Component");
			return components.find(std::type_index(typeid(T))) != components.end();
		}

		//用于移除组件
		template <typename T>
		void RemoveComponent() {
			static_assert(std::is_base_of<Component, T>::value, "T 必须继承自 Component");
			auto type_index = std::type_index(typeid(T));
			auto it = components.find(type_index);
			if (it != components.end()) {
				it->second->clean();
				components.erase(it);
			}
		}

		void Destroy();
		void Update(float);
		void Render();
		void HandleInput();
	};
}