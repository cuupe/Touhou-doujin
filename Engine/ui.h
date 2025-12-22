#pragma once
#include "GameObject.h"
#include "AudioManager.h"
#include "SceneManager.h"
namespace Engine::UISystem {
    class Panel {
    protected:
        std::string name;

    public:
        explicit Panel(const std::string& panel_name = "") : name(panel_name) {}
        virtual ~Panel() = default;

    public:
        virtual void Init(Engine::Core::Context&) = 0;
        virtual void HandleInput(Engine::Core::Context&, Engine::Audio::AudioManager&,
            Engine::Scene::SceneManager&) = 0;
        virtual void Update(float, Engine::Core::Context&) = 0;
        virtual void Render(Engine::Core::Context&) = 0;

    public:
        std::string GetName() const { return name; }

    public:
        virtual void OnEnter() {}
        virtual void OnLeave() {} 
        virtual void OnActivate() {}
    };
}