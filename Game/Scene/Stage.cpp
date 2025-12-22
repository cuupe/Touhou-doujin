#include "Stage.h"
#include "../Bullet/BulletManager.h"
#include "../../Engine/EffectManager.h"
#include "../../Engine/Components/ColliderComponent.h"
#include "../Engine/GameObject.h"
#include "../Engine/Components/TransformComponent.h"
#include "../Engine/Components/SpriteComponent.h"
#include "../Player/Player.h"
#include "../../Engine/InputManager.h"
#include "../Enemy/Enemy.h"
#include "../Bullet/Bullet.h"
#include "../../Engine/render.h"
#include "../../Engine/CameraManager.h"
#include "../../Engine/D3D/model.h"
#include "../../Engine/SceneManager.h"
#include "../../Engine/text.h"
#include "UIPanel/ScorePanel.h"
#include "UIPanel/PausePanel.h"
#include "TitleScene.h"
#include "../Item/ItemManager.h"
#include "../Player/Players/Reimu.h"
#include "UIPanel/OverPanel.h"
#include <nlohmann/json.hpp>
namespace Game::Scene {
	Stage::Stage(const std::string& name, Engine::Core::Context& ctx, Engine::Scene::SceneManager& s_m,
		Engine::Audio::AudioManager& _au, Engine::UISystem::UIManager& _ui)
		:Scene(name, ctx, s_m, _au, _ui)
	{ 
		pbm = std::make_unique<Game::Manager::PlayerBulletManager>(ctx, _au);
		em = std::make_unique<Game::Manager::EnemyManager>(ctx, _au);
		bm = std::make_unique<Manager::BulletManager>(ctx, _au);
		im = std::make_unique<Manager::ItemManager>(ctx, _au);
		player = std::make_unique<Game::Player::Reimu>(ctx, "pl00", _au, s_m, _ui, *pbm, *em, *bm);
		cm = std::make_unique<Engine::Render::CameraManager>();

		L = luaL_newstate();
		luaL_openlibs(L);
	}

	Stage::~Stage() {
		auto& r = ctx.GetRenderer();
		auto& desc = r.GetTexDesc();
		float height = static_cast<float>(desc.Height);
		float times = height / 960.0f;
		float dx = (static_cast<float>(desc.Width) - 1280.0f * times) / 2.0f;
		r.SetViewPort(0.0f + dx, 0.0f, 1280.0f * times, height);
		r.UpdateAspect(0.0f, 0.0f, 1280.0f, 960.0f);
		if (bg_script) {
			bg_script.reset();
		}
		if (script_level) {
			script_level.reset();
		}
		if (L) {
			lua_close(L);
		}
	}

	void Stage::Init()
	{
		try {
			script_level = std::make_unique<Game::Level::ScriptLevel>(
				ctx, au, *em, *bm, *player, this, L, "resources/lua_scripts/stage.lua");
			bg_script = std::make_unique<Game::Script::ScriptBackground>(
				ctx,
				L,
				"resources/lua_scripts/stagebcg.lua",
				"stage4bg", cm.get());

			std::ifstream ascii_file("data/stage.json");
			nlohmann::json json;
			ascii_file >> json;
			std::unique_ptr<Engine::Core::GameObject> gb = nullptr;
			for (const auto& bcg : json["stage_back"]) {
				gb = std::make_unique<Engine::Core::GameObject>();
				gb->AddComponent<Engine::Core::Components::TransformComponent>(
					std::move(Engine::Maths::Vec2{ bcg["position"]["x"], bcg["position"]["y"] }),
					std::move(Engine::Maths::Vec2{ bcg["scale"]["x"], bcg["scale"]["y"] }));
				std::string source = bcg["source"];
				gb->AddComponent<Engine::Core::Components::SpriteComponent>(source, ctx.GetResourceManager(),
					std::move(SDL_FRect{ bcg["rect"]["x"], bcg["rect"]["y"], bcg["rect"]["w"], bcg["rect"]["h"] }),
					bcg["align"]);
				AddGameObject(std::move(gb));
			}

			auto drop_handler = [this](const Vec2& pos, int id) {
				if (drop_table.count(id)) {
					im->SpawnItems(pos, drop_table[id]);
				}
				};

			bm->SetOnBulletDestroyCallback(drop_handler);
			em->SetOnEnemyKilledCallback(drop_handler);
			pbm->SetOnEnemyKilledCallback(drop_handler);

			auto camera = std::make_unique<Engine::Render::Camera>();
			camera->SetPositionFloat3({ 0.0f, 0.0f, -10.0f });
			camera->SetTargetFloat3({ 0.0f, 0.0f, 0.0f });
			camera->SetUpFloat3({ 0.0f, 1.0f, 0.0f });
			camera->SetFov(_1_2_PI);
			camera->SetAspect(775.0f / 900.0f);
			camera->SetNearFarZ(0.1f, 1000.0f);

			camera->SetFog(0.0f, 100.0f, { 0,0,0 }, false);
			cm->AddCamera("main", std::move(camera));
			ctx.GetRenderer().SetCameraManager(cm.get());
			bg_script->Init();
			script_level->Init();

			ui.RequestPushPanel(std::make_unique<UI::ScorePanel>("score", ui.GetTextManager()));

			RegisterDrop(Items::ItemType::SmallPower, 1);
			RegisterDrop(Items::ItemType::BigPower, 1);
			RegisterDrop(Items::ItemType::Point, 3);
			RegisterDrop(Items::ItemType::QuarterLife, 1);
			RegisterDrop(Items::ItemType::FullLife, 1);
			RegisterDrop({
				{Items::ItemType::QuarterLife, 1},
				{Items::ItemType::SmallPower, 20}
				});
			RegisterDrop({ {Items::ItemType::SmallPower, 3},
				{Items::ItemType::BigPower, 1} });

			is_init = true;
		}
		catch (const std::exception& err) {
			spdlog::error("初始化场景发生异常：{}", err.what());
			return;
		}
	}


	void Stage::HandleInput()
	{
		auto& input = ctx.GetInputManager();
		if (input.IsActionPressed("esc")) {
			auto ui_ptr = ui.GetCurrentPanel();
			if (!ui_ptr) {
				return;
			}

			if (ui_ptr->GetName() == "score") {
				is_pause = true;
				au.PlaySFX("se_pause", "select");
				ui.RequestPushPanel(std::make_unique<Game::UI::PausePanel>("pause", ui, this));

				if (au.IsBGMPlaying()) {
					au.PauseBGM();
				}
			}
		}

		if (!is_pause) {
			player->HandleInput();
		}
		ui.HandleInput();
	}

	void Stage::Update(float d_t)
	{
		if (!is_pause) {
			Scene::Update(d_t);
			bm->Update(d_t);
			em->Update(d_t);
			player->Update(d_t);
			im->Update(d_t);
			pbm->Update(d_t);
			if (bg_script){
				bg_script->Update(d_t);
			}
			if (script_level) {
				script_level->Update(d_t);
			}
			im->CheckCollision(player.get());
			bm->CheckCollisions(player.get());
			pbm->CheckCollisions(em->GetEnemies());
			if (em->CheckCollisions(player.get())) {
				bm->RemoveAllBullets(false);
			}

			if (player->IsOver()) {
				is_pause = true;
			}
		}
		ui.Update(d_t);

		Panel* current = ui.GetCurrentPanel();
		if (current) {
			if (is_pause && current->GetName() == "score") {
				is_pause = false;
				au.PlayBGM();
			}
		}
	}

	void Stage::Render()
	{
		auto& r = ctx.GetRenderer();
		auto& desc = r.GetTexDesc();
		float height = static_cast<float>(desc.Height);
		float times = height / 960.0f;
		float dx = (static_cast<float>(desc.Width) - 1280.0f * times) / 2.0f;
		

		SetBackGround(dx, height, times);
		r.Begin2D();
		Scene::Render();
		

		SetGameView(dx, height, times);
		r.Begin3D();
		RenderBackground();

		r.Begin2D();
		em->Render();
		bm->Render();
		im->Render();
		player->Render();
		pbm->Render();

		SetBackGround(dx, height, times);
		r.Begin2D();
		ui.Render();
	}

	void Stage::SetBackGround(float dx, float height, float times)
	{
		auto& r = ctx.GetRenderer();
		r.SetViewPort(0.0f + dx, 0.0f, 1280.0f * times, height);
		r.UpdateAspect(0.0f, 0.0f, 1280.0f, 960.0f);
	}

	void Stage::SetGameView(float dx, float height, float times)
	{
		auto& r = ctx.GetRenderer();
		r.SetViewPort(50.0f * times + dx, 30.0f * times, 775.0f * times, 900.0f * times);
		r.UpdateAspect(50.0f, 30.0f, 775.0f, 900.0f);
	}

	void Stage::RenderBackground()
	{
		if (bg_script) {
			bg_script->Render();
		}
	}


	int Stage::RegisterDrop(const std::vector<std::pair<Items::ItemType, int>>& reg)
	{
		int id = next_drop_id++;
		drop_table[id] = reg;
		return id;
	}

	int Stage::RegisterDrop(Items::ItemType type, int ir)
	{
		int id = next_drop_id++;
		drop_table[id] = { {type, ir} };
		return id;
	}

	void Stage::FinishStage() {
		if (!is_pause) {
			is_pause = true;
			ui.RequestPushPanel(std::make_unique<Game::UI::OverPanel>("over", ui));
		}
	}

	void Stage::Reset()
	{
		auto diff = gamedata.difficulty;
		auto id = gamedata.id;
		ResetData();
		gamedata.difficulty = diff;
		gamedata.id = id;
		switch (diff) {
		case 0:
			gamedata.life = 7 * 5;
			break;
		case 1:
			gamedata.life = 5 * 5;
			break;
		case 2:
			gamedata.life = 3 * 5;
			break;
		case 3:
			gamedata.life = 1 * 5;
			break;
		}
		ui.RequestPopPanel();
		ctx.GetInputManager().ResetAllState();
		bm->RemoveAllBullets(false);
		for (auto& e : em->GetEnemies()) {
			e->GetEnemyComponent()->GetEnemyData().is_active = false;
		}
		player->Reset();
		lua_getglobal(L, "task_manager");
		if (lua_istable(L, -1)) {
			lua_newtable(L);
			lua_setfield(L, -2, "tasks");
		}
		lua_pop(L, 1);

		if (script_level) {
			script_level->Init();
		}
		if (bg_script) {
			bg_script->Init();
		}
	}
}