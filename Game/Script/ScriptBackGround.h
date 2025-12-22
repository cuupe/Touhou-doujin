#pragma once
#include "../../prefix.h"
namespace Engine::Core { class Context; }
namespace Engine::Render { class CameraManager; }
namespace Engine::Resource { struct TextureResource; }

namespace Game::Script {
    struct Prop {
        std::string textureKey;
        float x, y, z;
        float scale;
    };

    class ScriptBackground {
    public:
        ScriptBackground(Engine::Core::Context& _ctx,
            lua_State* _L,
            const std::string& _path,
            const std::string& _name,
            Engine::Render::CameraManager* _cm);

        ~ScriptBackground();

        void Init();
        void Update(float dt);
        void Render();

        void SetTexture(const std::string& key);

    private:
        void CreateSandboxEnv();

    private:
        Engine::Core::Context& ctx;
        lua_State* L = nullptr;

        std::string path;
        std::string name;
        int env_ref = LUA_NOREF;

        Engine::Resource::TextureResource* tex = nullptr;
        Engine::Render::CameraManager* cm = nullptr;

        std::vector<Prop> props;

        static ScriptBackground* s_instance;

    public:
        static int API_SetImage(lua_State* L);
        static int API_Set3D(lua_State* L);
        static int API_SetImageState(lua_State* L);
        static int API_Render4V(lua_State* L);
        static int API_Render4VMix(lua_State* L);
        static int API_Stage_CreateProp(lua_State* L);
    };
}