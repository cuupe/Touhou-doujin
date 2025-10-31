#include "GameObject.h"

namespace Engine::Core {
    GameObject::GameObject(const std::string& name, const std::string& tag)
        :name(name), tag(tag)
    {
    }

    void GameObject::Update(float delta_time) {
        for (auto& pair : components) {
            pair.second->Update(delta_time);
        }
    }

    void GameObject::Render() {
        for (auto& pair : components) {
            pair.second->Render();
        }
    }


    void GameObject::Destroy() {
        spdlog::trace("销毁 GameObject 中...");
        for (auto& pair : components) {
            pair.second->Destory();
        }
        components.clear();
    }

    void GameObject::HandleInput() {
        for (auto& pair : components) {
            pair.second->HandleInput();
        }
    }
}