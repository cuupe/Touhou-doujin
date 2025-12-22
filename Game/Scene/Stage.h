#pragma once
#include "../GameData.h"
#include "../../Engine/scene.h"
#include "../Bullet/BulletManager.h"
#include "../Player/PlayerBulletManager.h"
#include "../Item/ItemManager.h" 
#include "../Enemy/EnemyManager.h"
#include "../Script/ScriptBackGround.h"
#include "../Script/ScriptLevel.h"
#include <wrl/client.h>
using Microsoft::WRL::ComPtr;
namespace Engine::Render {
	class CameraManager;
	class TextManager;
}
namespace Game::Entity {
	class Player;
}
namespace Engine::Audio {
	class AudioManager;
}
namespace Engine::UISystem {
	class UIManager;
}

namespace Game::Scene {
	class Stage final :public Engine::Scene::Scene {
	private:
		std::unique_ptr<Game::Manager::BulletManager> bm;
		std::unique_ptr<Game::Manager::EnemyManager> em;
		std::unique_ptr<Game::Manager::ItemManager> im;
		std::unique_ptr<Game::Manager::PlayerBulletManager> pbm;
		std::unique_ptr<Game::Entity::Player> player;
		std::unique_ptr<Engine::Render::CameraManager> cm;
		std::unique_ptr<Game::Script::ScriptBackground> bg_script;
		std::unique_ptr<Game::Level::ScriptLevel> script_level;
		std::map<int, std::vector<std::pair<Game::Items::ItemType, int>>> drop_table;
		lua_State* L = nullptr;
		int next_drop_id = 0;
	private:
		bool is_pause = false;

	public:
		Stage(const std::string& name,
			Engine::Core::Context& ctx,
			Engine::Scene::SceneManager& s_m,
			Engine::Audio::AudioManager& _au,
			Engine::UISystem::UIManager& _ui);
		~Stage();
	public:
		Game::Manager::BulletManager* GetBulletManager() const { return bm.get(); }
		Game::Manager::EnemyManager* GetEnemyManager() const { return em.get(); }
		Game::Manager::ItemManager* GetItemManager() const { return im.get(); }
		Game::Manager::PlayerBulletManager* GetPlayerBulletManager() const { return pbm.get(); }
		Game::Entity::Player* GetPlayer() const { return player.get(); }
		Engine::Render::CameraManager* GetCameraManager() const { return cm.get(); }
		lua_State* GetLuaState() const { return L; }
		
	public:
		void Init() override;
		void HandleInput() override;
		void Update(float) override;
		void Render() override;
		void Reset() override;

		void FinishStage();
	private:
		void SetBackGround(float, float, float);
		void SetGameView(float, float, float);
		void RenderBackground();
		int RegisterDrop(const std::vector<std::pair<Items::ItemType, int>>& reg);
		int RegisterDrop(Items::ItemType type, int ir);
		
	};
}