#pragma once
#include "../../Engine/context.h"
#include "../Enemy/EnemyManager.h"
#include "../Bullet/BulletManager.h"
#include "../prefix.h"
#include <string>
namespace Game::Scene {
    class Stage;
}
namespace Game::Level {
    class ScriptLevel {
    private:
        Engine::Core::Context& ctx;
        Engine::Audio::AudioManager& au;
        Game::Manager::EnemyManager& em;
        Game::Manager::BulletManager& bm;
        Game::Entity::Player& player;
        Game::Scene::Stage* stage = nullptr;
        lua_State* L = nullptr;
        std::string path;

    public:
        ScriptLevel(Engine::Core::Context& _ctx,
            Engine::Audio::AudioManager& _au,
            Game::Manager::EnemyManager& _em,
            Game::Manager::BulletManager& _bm,
            Game::Entity::Player& pl,
            Game::Scene::Stage* _stage,
            lua_State* _L,
            const std::string& _path);
        ~ScriptLevel();

        void Init();
        void Update(float dt);

    private:
        void RegisterAPI();
        static int API_CreateEnemy(lua_State* L);
        static int API_Enemy_SetPosition(lua_State* L);
        static int API_Enemy_GetPosition(lua_State* L);
        static int API_Enemy_IsValid(lua_State* L);
        static int API_Enemy_Despawn(lua_State* L);
        static int API_Enemy_Health(lua_State* L);
        static int API_CreateBullet(lua_State* L);
        static int API_ClearBullets(lua_State* L);
        static int API_GetAngleToPlayer(lua_State* L);
        static int API_PlayBGM(lua_State* L);
        static int API_PlaySFX(lua_State* L);
        static int API_FinishStage(lua_State* L);
        static ScriptLevel* GetInstance(lua_State* L);
    };
}