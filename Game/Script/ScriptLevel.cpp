#include "ScriptLevel.h"
#include "../Engine/AudioManager.h"
#include "../Scene/Stage.h"
namespace Game::Level {
    ScriptLevel::ScriptLevel(Engine::Core::Context& _ctx,
        Engine::Audio::AudioManager& _au,
        Game::Manager::EnemyManager& _em,
        Game::Manager::BulletManager& _bm,
        Game::Entity::Player& pl,
        Game::Scene::Stage* _stage,
        lua_State* _L,
        const std::string& _path)
        : ctx(_ctx), au(_au), em(_em), bm(_bm),
        player(pl), stage(_stage), L(_L), path(_path)
    {
        lua_pushstring(L, "__LevelPtr");
        lua_pushlightuserdata(L, this);
        lua_settable(L, LUA_REGISTRYINDEX);
        RegisterAPI();
    }

    ScriptLevel::~ScriptLevel() {
    }

    ScriptLevel* ScriptLevel::GetInstance(lua_State* L) {
        lua_pushstring(L, "__LevelPtr");
        lua_gettable(L, LUA_REGISTRYINDEX);
        auto* ptr = (ScriptLevel*)lua_touserdata(L, -1);
        lua_pop(L, 1);
        return ptr;
    }

    void ScriptLevel::RegisterAPI() {
        lua_register(L, "CreateEnemy", API_CreateEnemy);
        lua_register(L, "Enemy_SetPosition", API_Enemy_SetPosition);
        lua_register(L, "Enemy_GetPosition", API_Enemy_GetPosition);
        lua_register(L, "Enemy_IsValid", API_Enemy_IsValid);
        lua_register(L, "Enemy_Health", API_Enemy_Health);
        lua_register(L, "Enemy_Despawn", API_Enemy_Despawn);
        lua_register(L, "CreateBullet", API_CreateBullet);
        lua_register(L, "ClearBullets", API_ClearBullets);
        lua_register(L, "GetAngleToPlayer", API_GetAngleToPlayer);
        lua_register(L, "PlayBGM", API_PlayBGM);
        lua_register(L, "PlaySFX", API_PlaySFX);
        lua_register(L, "FinishStage", API_FinishStage);
    }

    void ScriptLevel::Init() {
        if (luaL_dofile(L, "resources/lua_scripts/task.lua") != LUA_OK) {
            spdlog::error("Lua核心错误: {}", lua_tostring(L, -1));
            lua_pop(L, 1);
        }

        if (luaL_dofile(L, path.c_str()) != LUA_OK) {
            spdlog::error("Lua脚本读取错误: {}", lua_tostring(L, -1));
            lua_pop(L, 1);
            return;
        }

        lua_getglobal(L, "level_init");
        if (lua_isfunction(L, -1)) {
            if (lua_pcall(L, 0, 0, 0) != LUA_OK) {
                spdlog::error("关卡初始化错误: {}", lua_tostring(L, -1));
                lua_pop(L, 1);
            }
        }
        else {
            lua_pop(L, 1);
        }
    }

    void ScriptLevel::Update(float d_t) {
        lua_getglobal(L, "level_update");
        if (lua_isfunction(L, -1)) {
            lua_pushnumber(L, d_t);
            if (lua_pcall(L, 1, 0, 0) != LUA_OK) {
                spdlog::error("关卡更新错误: {}", lua_tostring(L, -1));
                lua_pop(L, 1);
            }
        }
        else {
            lua_pop(L, 1);
        }
    }


    int ScriptLevel::API_CreateEnemy(lua_State* L) {
        auto self = GetInstance(L);

        const char* id = luaL_checkstring(L, 1);
        float x = (float)luaL_checknumber(L, 2);
        float y = (float)luaL_checknumber(L, 3);
        int health = (int)luaL_checkinteger(L, 4);
        int drop_id = (int)luaL_optinteger(L, 5, 0);

        int enemy_id = self->em.SpawnEnemy(id, { x, y }, health, drop_id);
        lua_pushinteger(L, enemy_id);
        return 1;
    }

    int ScriptLevel::API_Enemy_SetPosition(lua_State* L) {
        auto self = GetInstance(L);

        int id = (int)luaL_checkinteger(L, 1);
        float x = (float)luaL_checknumber(L, 2);
        float y = (float)luaL_checknumber(L, 3);

        auto enemy = self->em.GetEnemyByIndex(id);

        if (enemy) {
            auto go = enemy->GetGameObject();
            if (go) {
                auto* enemyComp = go->GetComponent<Game::Component::EnemyComponent>();
                if (enemyComp) {
                    enemyComp->GetEnemyData().position = { x, y }; 
                }

                auto* transComp = go->GetComponent<Engine::Core::Components::TransformComponent>();
                if (transComp) {
                    transComp->SetPosition({ x, y });
                }

            }
        }

        return 0;
    }

    int ScriptLevel::API_Enemy_GetPosition(lua_State* L) {
        auto* self = GetInstance(L);
        int id = (int)luaL_checkinteger(L, 1);

        auto enemy = self->em.GetEnemyByIndex(id);

        if (enemy) {
            auto comp = enemy->GetGameObject()->GetComponent<Game::Component::EnemyComponent>();
            if (comp) {
                const auto& pos = comp->GetEnemyData().position;
                lua_pushnumber(L, pos.x);
                lua_pushnumber(L, pos.y);
                return 2;
            }
        }

        lua_pushnil(L);
        lua_pushnil(L);
        return 2;
    }

    int ScriptLevel::API_Enemy_IsValid(lua_State* L) {
        auto self = GetInstance(L);
        int id = (int)luaL_checkinteger(L, 1);

        auto enemy = self->em.GetEnemyByIndex(id);
        lua_pushboolean(L, enemy != nullptr);
        return 1;
    }

    int ScriptLevel::API_Enemy_Despawn(lua_State* L) {
        auto self = GetInstance(L);
        int id = (int)luaL_checkinteger(L, 1);
        auto enemy = self->em.GetEnemyByIndex(id);
        if (enemy) {
            enemy->Despawn();
        }
        return 0;
    }

    int ScriptLevel::API_Enemy_Health(lua_State* L) {
        auto self = GetInstance(L);
        int id = (int)luaL_checkinteger(L, 1);
        auto enemy = self->em.GetEnemyByIndex(id);
        if (enemy) {
            lua_pushnumber(L, enemy->GetEnemyComponent()->GetEnemyData().health);
        }
        return 1;
    }

    int ScriptLevel::API_CreateBullet(lua_State* L) {
        auto self = GetInstance(L);

        const char* id = luaL_checkstring(L, 1);
        float x = (float)luaL_checknumber(L, 2);
        float y = (float)luaL_checknumber(L, 3);
        float speed = (float)luaL_checknumber(L, 4);
        float angle = (float)luaL_checknumber(L, 5);

        float speed_acc = (float)luaL_optnumber(L, 6, 0.0f);
        float angle_acc = (float)luaL_optnumber(L, 7, 0.0f);
        int offset = (int)luaL_optinteger(L, 8, 0);

        self->bm.SpawnSingleBullet(id, { x, y }, speed, angle, speed_acc, angle_acc, offset);
        return 0;
    }

    int ScriptLevel::API_GetAngleToPlayer(lua_State* L) {
        auto self = GetInstance(L);
        float x = (float)luaL_checknumber(L, 1);
        float y = (float)luaL_checknumber(L, 2);

        float angle = self->player.AngleToPlayer({ x, y });
        lua_pushnumber(L, Engine::Maths::RadToDeg(angle));
        return 1;
    }

    int ScriptLevel::API_PlayBGM(lua_State* L)
    {
        auto self = GetInstance(L);
        const char* id = luaL_checkstring(L, 1, nullptr);
        if (id) {
            self->au.PlayBGM(id);
        }
        else {
            self->au.PlayBGM();
        }
        return 0;
    }

    int ScriptLevel::API_PlaySFX(lua_State* L)
    {
        auto self = GetInstance(L);
        const char* id = luaL_checkstring(L, 1, nullptr);
        const char* track = luaL_checkstring(L, 2, nullptr);
        if (!id || !track) {
            spdlog::error("不全的音效或轨道信息");
            return 0;
        }
        self->au.PlaySFX(id, track);
        return 0;
    }

    int ScriptLevel::API_FinishStage(lua_State* L)
    {
        auto self = GetInstance(L);
        if (self->stage) {
            self->stage->FinishStage();
        }
        return 0;
    }

    int ScriptLevel::API_ClearBullets(lua_State* L) {
        auto* self = GetInstance(L);
        bool turn = lua_toboolean(L, 1);
        int id = (int)luaL_optnumber(L, 2, 2);
        self->bm.RemoveAllBullets(turn, 2);
        return 0;
    }
}