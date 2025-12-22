#include "ScriptBackground.h"
#include <spdlog/spdlog.h>
#include <filesystem>
#include <cmath>
#include "../../Engine/context.h"
#include "../../Engine/resources.h"
#include "../../Engine/render.h"
#include "../../Engine/CameraManager.h"
#include "../../Engine/camera.h"

namespace Game::Script {
    ScriptBackground* ScriptBackground::s_instance = nullptr;

    ScriptBackground::ScriptBackground(Engine::Core::Context& _ctx,
        lua_State* _L,
        const std::string& _path,
        const std::string& _name,
        Engine::Render::CameraManager* _cm)
        : ctx(_ctx), L(_L), path(_path), name(_name),
        cm(_cm)
    {
        s_instance = this;
    }

    ScriptBackground::~ScriptBackground() {
        if (L && env_ref != LUA_NOREF) {
            luaL_unref(L, LUA_REGISTRYINDEX, env_ref);
            env_ref = LUA_NOREF;
        }
        if (s_instance == this) {
            s_instance = nullptr;
        }
    }

    void ScriptBackground::CreateSandboxEnv() {
        if (!L) return;

        lua_newtable(L);

        lua_newtable(L);
        lua_getglobal(L, "_G");
        lua_setfield(L, -2, "__index");
        lua_setmetatable(L, -2);

        lua_pushcfunction(L, API_Set3D);           lua_setglobal(L, "Set3D");
        lua_pushcfunction(L, API_SetImageState);   lua_setglobal(L, "SetImageState");
        lua_pushcfunction(L, API_Render4V);        lua_setglobal(L, "Render4V");
        lua_pushcfunction(L, API_Render4VMix);     lua_setglobal(L, "Render4VMix");

        lua_newtable(L);
        lua_pushcfunction(L, [](lua_State* L) -> int {
            return 0;
            });
        lua_setfield(L, -2, "SetImage");
        lua_setfield(L, -2, "Background");
    }

    void ScriptBackground::Init() {
        if (!L) return;

        if (luaL_loadfile(L, path.c_str()) != LUA_OK) {
            spdlog::error("Lua Load Error: {}", lua_tostring(L, -1));
            lua_pop(L, 1);
            return;
        }

        CreateSandboxEnv();
        lua_pushvalue(L, -1);
        env_ref = luaL_ref(L, LUA_REGISTRYINDEX);

#if LUA_VERSION_NUM >= 502
        lua_setupvalue(L, -2, 1);
#else
        lua_setfenv(L, -2);
#endif

        if (lua_pcall(L, 0, 0, 0) != LUA_OK) {
            spdlog::error("Lua Run Error: {}", lua_tostring(L, -1));
            lua_pop(L, 1);
            return;
        }

        lua_rawgeti(L, LUA_REGISTRYINDEX, env_ref);
        lua_getfield(L, -1, "Init");
        if (lua_isfunction(L, -1)) {
            if (lua_pcall(L, 0, 0, 0) != LUA_OK) {
                spdlog::error("Lua Init Call Error: {}", lua_tostring(L, -1));
                lua_pop(L, 1);
            }
        }
        else lua_pop(L, 1);
        lua_pop(L, 1);
    }

    void ScriptBackground::Update(float dt) {
        if (!L || env_ref == LUA_NOREF) return;
        lua_rawgeti(L, LUA_REGISTRYINDEX, env_ref);
        lua_getfield(L, -1, "Update");
        if (lua_isfunction(L, -1)) {
            lua_pushnumber(L, dt);
            if (lua_pcall(L, 1, 0, 0) != LUA_OK) {
                spdlog::error("Lua Update Call Error: {}", lua_tostring(L, -1));
                lua_pop(L, 1);
            }
        }
        else lua_pop(L, 1);
        lua_pop(L, 1);
    }

    void ScriptBackground::Render() {
        if (!L || env_ref == LUA_NOREF) {
            return;
        }

        auto& renderer = ctx.GetRenderer();

        lua_rawgeti(L, LUA_REGISTRYINDEX, env_ref);
        lua_getfield(L, -1, "Render");
        if (lua_isfunction(L, -1)) {
            if (lua_pcall(L, 0, 0, 0) != LUA_OK) {
                spdlog::error("Lua Render 错误: {}", lua_tostring(L, -1));
                lua_pop(L, 1);
            }
        }
        else {
            lua_pop(L, 1);
        }
        lua_pop(L, 1);

        auto& res = ctx.GetResourceManager();
        auto* cam = cm->GetActiveCamera();

        if (!cam) return;
        DirectX::XMVECTOR camPos = cam->GetPosition();
        DirectX::XMVECTOR up = DirectX::XMVectorSet(0, 1, 0, 0);

        for (const auto& prop : props) {
            auto* tex = res.GetTexture(prop.textureKey);
            if (!tex) continue;

            const float PIXELS_PER_UNIT = 100.0f;
            float worldW = (tex->width * prop.scale) / PIXELS_PER_UNIT;
            float worldH = (tex->height * prop.scale) / PIXELS_PER_UNIT;

            DirectX::XMVECTOR objPos = DirectX::XMVectorSet(prop.x, prop.y, prop.z, 0);
            DirectX::XMVECTOR look = DirectX::XMVectorSubtract(camPos, objPos);
            look = DirectX::XMVectorSetY(look, 0.0f);
            look = DirectX::XMVector3Normalize(look);

            DirectX::XMVECTOR right = DirectX::XMVector3Cross(up, look);
            right = DirectX::XMVector3Normalize(right);


            DirectX::XMVECTOR halfW = DirectX::XMVectorScale(right, worldW * 0.5f);
            DirectX::XMVECTOR fullH = DirectX::XMVectorScale(up, worldH);

            DirectX::XMVECTOR p1 = DirectX::XMVectorAdd(DirectX::XMVectorSubtract(objPos, halfW), fullH);
            DirectX::XMVECTOR p2 = DirectX::XMVectorAdd(DirectX::XMVectorAdd(objPos, halfW), fullH);
            DirectX::XMVECTOR p3 = DirectX::XMVectorSubtract(objPos, halfW);
            DirectX::XMVECTOR p4 = DirectX::XMVectorAdd(objPos, halfW);

            DirectX::XMFLOAT3 v1, v2, v3, v4;
            DirectX::XMStoreFloat3(&v1, p1);
            DirectX::XMStoreFloat3(&v2, p2);
            DirectX::XMStoreFloat3(&v3, p3);
            DirectX::XMStoreFloat3(&v4, p4);
            DirectX::XMFLOAT2 uv1 = { 0, 0 };
            DirectX::XMFLOAT2 uv2 = { 1, 0 };
            DirectX::XMFLOAT2 uv3 = { 0, 1 };
            DirectX::XMFLOAT2 uv4 = { 1, 1 };

            renderer.Draw3DQuad(prop.textureKey, "", 0.0f, v1, uv1, v2, uv2, v3, uv3, v4, uv4);
        }
    }

    void ScriptBackground::SetTexture(const std::string& key) {
        auto& res = ctx.GetResourceManager();
        auto* tex = res.GetTexture(key);
        if (tex) {
            tex = tex;
        }
    }

    int ScriptBackground::API_Stage_CreateProp(lua_State* L) {
        if (!s_instance) {
            return 0;
        }

        const char* texName = luaL_checkstring(L, 1);
        float x = (float)luaL_checknumber(L, 2);
        float y = (float)luaL_checknumber(L, 3);
        float z = (float)luaL_checknumber(L, 4);
        float scale = (float)luaL_optnumber(L, 5, 1.0f);

        Prop p;
        p.textureKey = texName;
        p.x = x;
        p.y = y;
        p.z = z;
        p.scale = scale;

        s_instance->props.push_back(p);

        return 0;
    }

    int ScriptBackground::API_Set3D(lua_State* L) {
        if (!s_instance) return 0;
        const char* key = luaL_checkstring(L, 1);
        auto& renderer = s_instance->ctx.GetRenderer();

        std::string k(key);
        if (k == "fog") {
            float start = (float)luaL_checknumber(L, 2);
            float end = (float)luaL_checknumber(L, 3);
            float r = (float)luaL_optnumber(L, 4, 0);
            float g = (float)luaL_optnumber(L, 5, 0);
            float b = (float)luaL_optnumber(L, 6, 0);

            renderer.SetCameraParameterFog(start, end, { r / 255.0f, g / 255.0f, b / 255.0f }, true);
        }
        else if (k == "fovy") {
            renderer.SetCameraParameterFov((float)luaL_checknumber(L, 2));
        }
        else if (k == "z") {
            renderer.SetCameraParameterZ((float)luaL_checknumber(L, 2), (float)luaL_checknumber(L, 3));
        }
        else {
            renderer.SetCameraParameter(k, (float)luaL_checknumber(L, 2), 
                (float)luaL_checknumber(L, 3), (float)luaL_checknumber(L, 4));
        }
        return 0;
    }

    int ScriptBackground::API_SetImageState(lua_State* L) {
        if (!s_instance) return 0;
        const char* id = luaL_checkstring(L, 1);
        const char* mode = luaL_checkstring(L, 2);

        float r = (float)luaL_optnumber(L, 3, 255) / 255.0f;
        float g = (float)luaL_optnumber(L, 4, 255) / 255.0f;
        float b = (float)luaL_optnumber(L, 5, 255) / 255.0f;
        float a = (float)luaL_optnumber(L, 6, 255) / 255.0f;

        s_instance->ctx.GetRenderer().SetQuadRenderState(id, mode, { r, g, b, a });
        return 0;
    }

    int ScriptBackground::API_Render4V(lua_State* L) {
        if (!s_instance) return 0;

        if (lua_gettop(L) < 13) {
            return 0;
        }
        const char* id = luaL_checkstring(L, 1);

        DirectX::XMFLOAT3 v_tl = { (float)luaL_checknumber(L, 2),  
            (float)luaL_checknumber(L, 3),  (float)luaL_checknumber(L, 4) };
        DirectX::XMFLOAT3 v_tr = { (float)luaL_checknumber(L, 5),  
            (float)luaL_checknumber(L, 6),  (float)luaL_checknumber(L, 7) };
        DirectX::XMFLOAT3 v_br = { (float)luaL_checknumber(L, 8),  
            (float)luaL_checknumber(L, 9),  (float)luaL_checknumber(L, 10) };
        DirectX::XMFLOAT3 v_bl = { (float)luaL_checknumber(L, 11), 
            (float)luaL_checknumber(L, 12), (float)luaL_checknumber(L, 13) };

        DirectX::XMFLOAT3 p1 = v_tl;
        DirectX::XMFLOAT3 p2 = v_tr;
        DirectX::XMFLOAT3 p3 = v_br;
        DirectX::XMFLOAT3 p4 = v_bl;
        DirectX::XMFLOAT2 uv1 = { 0.0f, 0.0f };
        DirectX::XMFLOAT2 uv2 = { 1.0f, 0.0f };
        DirectX::XMFLOAT2 uv3 = { 1.0f, 1.0f };
        DirectX::XMFLOAT2 uv4 = { 0.0f, 1.0f };

        s_instance->ctx.GetRenderer().Draw3DQuad(id, "", 0.0f, p1, uv1, p2, uv2, p3, uv3, p4, uv4);

        return 0;
    }


    int ScriptBackground::API_Render4VMix(lua_State* L) {
        if (!s_instance) {
            return 0;
        }

        if (lua_gettop(L) < 15) return 0;

        const char* id_base = luaL_checkstring(L, 1);
        const char* id_mix = luaL_checkstring(L, 2);
        float factor = (float)luaL_checknumber(L, 3);
        DirectX::XMFLOAT3 v_tl = { (float)luaL_checknumber(L, 4),  (float)luaL_checknumber(L, 5),  (float)luaL_checknumber(L, 6) };
        DirectX::XMFLOAT3 v_tr = { (float)luaL_checknumber(L, 7),  (float)luaL_checknumber(L, 8),  (float)luaL_checknumber(L, 9) };
        DirectX::XMFLOAT3 v_br = { (float)luaL_checknumber(L, 10), (float)luaL_checknumber(L, 11), (float)luaL_checknumber(L, 12) };
        DirectX::XMFLOAT3 v_bl = { (float)luaL_checknumber(L, 13), (float)luaL_checknumber(L, 14), (float)luaL_checknumber(L, 15) };
        DirectX::XMFLOAT3 p1 = v_tl;
        DirectX::XMFLOAT3 p2 = v_tr;
        DirectX::XMFLOAT3 p3 = v_br;
        DirectX::XMFLOAT3 p4 = v_bl;
        DirectX::XMFLOAT2 uv1 = { 0.0f, 0.0f };
        DirectX::XMFLOAT2 uv2 = { 1.0f, 0.0f };
        DirectX::XMFLOAT2 uv3 = { 1.0f, 1.0f };
        DirectX::XMFLOAT2 uv4 = { 0.0f, 1.0f };

        s_instance->ctx.GetRenderer().Draw3DQuad(
            id_base,
            id_mix,
            factor,
            p1, uv1, p2, uv2, p3, uv3, p4, uv4
        );

        return 0;
    }

    int ScriptBackground::API_SetImage(lua_State* L) {
        if (s_instance) s_instance->SetTexture(luaL_checkstring(L, 1));
        return 0;
    }
}