#include "GameObject.h"

namespace Engine::Core {
    GameObject::GameObject(const std::string& name, const std::string& tag)
        :name(name), tag(tag)
    {
    }

    void GameObject::Update(float delta_time, Context& ctx) {
        for (auto& pair : components) {
            pair.second->Update(delta_time, ctx);
        }
    }

    void GameObject::Render(Context& ctx) {
        for (auto& pair : components) {
            pair.second->Render(ctx);
        }
    }

    void GameObject::Destroy() {
        spdlog::trace("销毁 GameObject 中...");
        for (auto& pair : components) {
            pair.second->Destory();
        }
        components.clear();
    }

    void GameObject::HandleInput(Context& ctx) {
        for (auto& pair : components) {
            pair.second->HandleInput(ctx);
        }
    }
}