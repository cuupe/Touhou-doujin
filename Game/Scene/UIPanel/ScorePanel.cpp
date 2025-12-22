#include "ScorePanel.h"
#include "../../../Engine/render.h"
#include "../../../Engine/Components/TransformComponent.h"
#include "../../../Engine/Components/SpriteComponent.h"
#include "../.../../Engine/resources.h"
#include "../../../Engine/context.h"
#include "../../../Engine/text.h"
#include "../../../Engine/D3D/model.h"
#include "../../GameData.h"
#include <nlohmann/json.hpp>
namespace Game::UI {
	void ScorePanel::Init(Engine::Core::Context& ctx)
	{
		nlohmann::json json;
		try {
			std::ifstream panel_data("data/score_panel.json");
			if (!panel_data.is_open()) {
				spdlog::error("无法打开 data/score_panel.json");
				return;
			}
			panel_data >> json;

			data_show.reserve(32);
			for (const auto& e : json.at("data_panel")) {
				auto gb = std::make_unique<Engine::Core::GameObject>();
				gb->AddComponent<Engine::Core::Components::TransformComponent>(
					Vec2{ e.at("pos").at(0).get<f32>(), e.at("pos").at(1).get<f32>() }
				);

				std::string str = json.at("source").at(0).get<std::string>();
				gb->AddComponent<Engine::Core::Components::SpriteComponent>(
					std::move(str),
					ctx.GetResourceManager(),
					SDL_FRect{
						e.at("rect").at(0).get<f32>(),
						e.at("rect").at(1).get<f32>(),
						e.at("rect").at(2).get<f32>(),
						e.at("rect").at(3).get<f32>()
					},
					Engine::Maths::Align::CENTER_LEFT,
					Vec2{ e.at("scale").at(0).get<f32>(), e.at("scale").at(1).get<f32>() }
				);

				data_show.push_back(std::move(gb));
				gb = std::make_unique<Engine::Core::GameObject>();
				gb->AddComponent<Engine::Core::Components::TransformComponent>(
					Vec2{ e.at("pos").at(0).get<f32>(), e.at("pos").at(1).get<f32>() });

				str = json.at("source").at(1).get<std::string>();
				gb->AddComponent<Engine::Core::Components::SpriteComponent>(
					std::move(str),
					ctx.GetResourceManager(),
					std::nullopt,
					Engine::Maths::Align::CENTER_LEFT,
					Vec2{ 1.0f, 0.8f }
				);
				data_show.push_back(std::move(gb));
			}
			const auto& life = json.at("life");
			life_config.offset = life.at("offset").get<f32>();
			life_config.scale = {
				life.at("scale").at(0).get<f32>(),
				life.at("scale").at(1).get<f32>()
			};
			life_config.base_position = {
				life.at("base_pos").at(0).get<f32>(),
				life.at("base_pos").at(1).get<f32>()
			};
			for (const auto& rect : life.at("rects")) {
				life_config.rects.push_back(SDL_FRect{
					rect.at(0).get<f32>(),
					rect.at(1).get<f32>(),
					rect.at(2).get<f32>(),
					rect.at(3).get<f32>()
					});
			}
			const auto& title = json.at("title");
			auto gb = std::make_unique<Engine::Core::GameObject>();
			gb->AddComponent<Engine::Core::Components::TransformComponent>(
				Vec2{ title.at("pos").at(0).get<f32>(), title.at("pos").at(1).get<f32>() }
			);

			std::string str = json.at("source").at(2).get<std::string>();

			gb->AddComponent<Engine::Core::Components::SpriteComponent>(
				std::move(str),
				ctx.GetResourceManager(),
				SDL_FRect{
					title.at("rect").at(0).get<f32>(),
					title.at("rect").at(1).get<f32>(),
					title.at("rect").at(2).get<f32>(),
					title.at("rect").at(3).get<f32>()
				},
				Engine::Maths::Align::CENTER_LEFT,
				Vec2{ title.at("scale").at(0).get<f32>(), title.at("scale").at(1).get<f32>() }
			);
			data_show.push_back(std::move(gb));
		}
		catch (const std::exception& err) {
			throw std::runtime_error(err.what());
			return;
		}
	}

	void ScorePanel::Update(f32 dt, Engine::Core::Context& ctx)
	{
		if (gamedata.score >= gamedata.high_score) {
			gamedata.high_score = gamedata.score;
		}
	}

	void ScorePanel::Render(Engine::Core::Context& ctx)
	{
		for (auto& p : data_show) {
			p->Render(ctx);
		}
		RenderData(ctx);
	}

	void ScorePanel::OnEnter()
	{ }

	void ScorePanel::OnActivate()
	{ }

	void ScorePanel::OnLeave()
	{ }

	void ScorePanel::RenderData(Engine::Core::Context& ctx)
	{
		tm.RenderTextASCII(std::to_string(gamedata.high_score), { 1220.0f, 150.0f }, { 1.2f, 1.2f }, -5.0f);
		tm.RenderTextASCII(std::to_string(gamedata.score), { 1220.0f, 210.0f }, { 1.2f, 1.2f }, -5.0f);
		tm.RenderTextASCII(std::to_string(gamedata.graze), { 1220.0f, 300.0f }, { 1.2f, 1.2f }, -5.0f);
		tm.RenderTextASCII(std::to_string(gamedata.power).substr(0, 3) + "/4.0"
			, { 1220.0f, 420.0f }, { 1.0f, 1.0f }, -5.0f);
		u8 life = gamedata.life;
        auto& r = ctx.GetRenderer();
        auto& res = ctx.GetResourceManager();

        auto PushQuad = [](std::vector<D3D::Vertex>& verts, const SDL_FRect& src, const SDL_FRect& dst, float texW, float texH) {
            float u0 = src.x / texW;
            float v0 = src.y / texH;
            float u1 = (src.x + src.w) / texW;
            float v1 = (src.y + src.h) / texH;

            DirectX::XMFLOAT4 color = { 1.0f, 1.0f, 1.0f, 1.0f };

            verts.push_back({ {dst.x, dst.y, 0.0f},             {u0, v0}, color });
            verts.push_back({ {dst.x + dst.w, dst.y, 0.0f},     {u1, v0}, color });
            verts.push_back({ {dst.x + dst.w, dst.y + dst.h, 0.0f}, {u1, v1}, color });
            verts.push_back({ {dst.x + dst.w, dst.y + dst.h, 0.0f}, {u1, v1}, color });
            verts.push_back({ {dst.x, dst.y + dst.h, 0.0f},     {u0, v1}, color });
            verts.push_back({ {dst.x, dst.y, 0.0f},             {u0, v0}, color });
            };
        auto texture_res = res.GetTexture("hint");
        if (!texture_res) {
            spdlog::error("不存在texture : hint");
        }
        else {
            u8 life = gamedata.life;
            u8 qut = life / 5;
            u8 af = life % 5;
            std::vector<D3D::Vertex> life_vertices;
            life_vertices.reserve((qut + 1) * 6);

            float texW = static_cast<float>(texture_res->width);
            float texH = static_cast<float>(texture_res->height);
            float commu_offset = 0.0f;
            for (u8 i = 0; i < qut; ++i) {
                SDL_FRect source_rect = life_config.rects.at(5);
                SDL_FRect dst_rect = {
                    life_config.base_position.x + commu_offset,
                    life_config.base_position.y - source_rect.h * life_config.scale.y / 2.0f,
                    source_rect.w * life_config.scale.x,
                    source_rect.h * life_config.scale.y
                };
                PushQuad(life_vertices, source_rect, dst_rect, texW, texH);
                commu_offset += life_config.offset;
            }
            if (af != 0) {
                SDL_FRect source_rect = life_config.rects.at(af);
                SDL_FRect dst_rect = {
                    life_config.base_position.x + commu_offset,
                    life_config.base_position.y - source_rect.h * life_config.scale.y / 2.0f,
                    source_rect.w * life_config.scale.x,
                    source_rect.h * life_config.scale.y
                };
                PushQuad(life_vertices, source_rect, dst_rect, texW, texH);
            }
			if (!(qut || af)) {
				SDL_FRect source_rect = life_config.rects.at(0);
				SDL_FRect dst_rect = {
					life_config.base_position.x + commu_offset,
					life_config.base_position.y - source_rect.h * life_config.scale.y / 2.0f,
					source_rect.w * life_config.scale.x,
					source_rect.h * life_config.scale.y
				};
				PushQuad(life_vertices, source_rect, dst_rect, texW, texH);
			}
            if (!life_vertices.empty()) {
                r.DrawVertices(texture_res->d3d_srv.Get(), life_vertices);
            }
        }

        texture_res = res.GetTexture("rank");
        if (texture_res) {
            std::vector<D3D::Vertex> rank_vertices;
            float texW = static_cast<float>(texture_res->width);
            float texH = static_cast<float>(texture_res->height);

            SDL_FRect source_rect = { 0.0f, gamedata.difficulty * 32.0f, 144.0f, 32.0f };
            SDL_FRect dst_rect = {
                1050.0f - source_rect.w,
                550.0f - source_rect.h,
                source_rect.w * 2,
                source_rect.h * 2
            };

            PushQuad(rank_vertices, source_rect, dst_rect, texW, texH);
            r.DrawVertices(texture_res->d3d_srv.Get(), rank_vertices);
        }
	}
}