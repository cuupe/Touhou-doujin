#include "GameObject.h"

namespace Engine::Core {
    GameObject::GameObject(const std::string& name, const std::string& tag)
        :name(name), tag(tag)
    { }

    void GameObject::Update(float delta_time, Context& ctx) {
        for (auto& pair : components) {
            if (!(pair.second->need_destroy)) {
                pair.second->Update(delta_time, ctx);
            }
            else {
                pending_destroy.push_back(pair.second.get());
            }
        }

        if (pending_destroy.size()) {
            for (auto& del : pending_destroy) {
                auto type_idx = std::type_index(typeid(*del));
                components.erase(type_idx);
            }
            pending_destroy.clear();
        }
    }

    void GameObject::Render(Context& ctx) {
        for (auto& pair : components) {
            pair.second->Render(ctx);
        }
    }

    void GameObject::Destroy() {
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