#include "PlayerDataPanel.h"
#include "../../../Engine/Components/TransformComponent.h"
#include "../../../Engine/Components/SpriteComponent.h"
#include "../../../Engine/EffectManager.h"
#include "../../../Engine/InputManager.h"
#include "../../../Engine/UIManager.h"
#include "../../../Engine/AudioManager.h"
#include "../../../Engine/schedule.h"
#include "../../../Engine/maths.h"
#include "../../../Engine/text.h"
#include "../../../Engine/context.h"
#include <fstream>
#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>
#include <algorithm>
#include <iomanip>
#include <sstream>

namespace Game::UI {
    using json = nlohmann::json;

    void PlayerDataPanel::Init(Engine::Core::Context& ctx)
    {
        LoadScores();
        try {
            std::ifstream f("data/player_data_panel.json");
            json config;
            if (f.is_open()) {
                f >> config;
            }
            else {
                spdlog::error("无法打开 data/player_data_panel.json");
                return;
            }
            auto& information = config.at("player_data_panel");

            std::unique_ptr<Engine::Core::GameObject> info_bcg = std::make_unique<Engine::Core::GameObject>();
            info_bcg->AddComponent<Engine::Core::Components::TransformComponent>(
                Engine::Maths::Vec2{ information.at("position").at(0).get<f32>(),
                                     information.at("position").at(1).get<f32>() });
            float bg_alpha_val = information.contains("alpha") ? information.at("alpha").get<float>() : 0.8f;
            info_bcg->AddComponent<Engine::Core::Components::SpriteComponent>(
                information.at("source").get<std::string>(), ctx.GetResourceManager(),
                SDL_FRect{ information.at("rect").at(0).get<f32>(),
                           information.at("rect").at(1).get<f32>(),
                           information.at("rect").at(2).get<f32>(),
                           information.at("rect").at(3).get<f32>() },
                Engine::Maths::Align::NONE,
                Engine::Maths::Vec2{ 1.0f, 1.0f },
                SDL_Color{ 255, 255, 255, 0 });

            info_bcgs.push_back(std::move(info_bcg));

            auto& title_config = information.at("title_config");
            info.title = information.at("title").get<std::string>();
            info.title_config.font_size = title_config.at("font_size").get<f32>();
            info.title_config.color = SDL_Color{
                title_config.at("color").at(0).get<u8>(),
                title_config.at("color").at(1).get<u8>(),
                title_config.at("color").at(2).get<u8>(),
                title_config.at("color").at(3).get<u8>()};
            info.title_config.line_spacing = title_config.at("line_spacing").get<f32>();
            auto& content_config = information.at("content_config");
            info.content_config.font_size = content_config.at("font_size").get<f32>();
            info.content_config.color = SDL_Color{
                content_config.at("color").at(0).get<u8>(),
                content_config.at("color").at(1).get<u8>(),
                content_config.at("color").at(2).get<u8>(),
                content_config.at("color").at(3).get<u8>()
            };
            info.content_config.line_spacing = content_config.at("line_spacing").get<f32>();

            std::unique_ptr<Engine::Core::GameObject> gb = std::make_unique<Engine::Core::GameObject>();
            gb->AddComponent<Engine::Core::Components::TransformComponent>(
                std::move(Engine::Maths::Vec2{ information["base_position"][0], information["base_position"][1] }));
            gb->AddComponent<Engine::Core::Components::SpriteComponent>(
                information["selected_sprite"]["source"], ctx.GetResourceManager(), std::move(SDL_FRect{
                    information["selected_sprite"]["rect"][0],information["selected_sprite"]["rect"][1],
                    information["selected_sprite"]["rect"][2],information["selected_sprite"]["rect"][3] }));

            Engine::Render::Effect::EffectManager::Blink(gb.get(), 3.0f);
            opts.push_back(std::move(gb));
        }
        catch (const std::exception& err) {
            throw std::runtime_error(err.what());
            return;
        }
        alpha = 0.0f;
        is_init = true;
    }

    void PlayerDataPanel::LoadScores() {
        rankings.clear();
        std::ifstream in("data/score.dat", std::ios::binary);
        if (in.is_open()) {
            ScoreEntry entry;
            while (in.read(reinterpret_cast<char*>(&entry), sizeof(ScoreEntry))) {
                rankings.push_back(entry);
                if (rankings.size() >= MAX_RANKINGS) break;
            }
            in.close();
        }
        std::sort(rankings.begin(), rankings.end(), [](const ScoreEntry& a, const ScoreEntry& b) {
            return a.score > b.score;
            });
    }

    void PlayerDataPanel::HandleInput(Engine::Core::Context& ctx,
        Engine::Audio::AudioManager& au, Engine::Scene::SceneManager& sm)
    {
        if (!is_init) {
            return;
        }

        auto& input = ctx.GetInputManager();

        if (input.IsActionPressed("esc") || input.IsActionPressed("ok") || input.IsActionPressed("bomb")) {
            is_init = false;
            au.PlaySFX("se_cancel00", "select");

            if (!opts.empty()) {
                Engine::Render::Effect::EffectManager::OKFlash(opts[0].get());
            }

            
            ctx.GetSchedule().NewSequence().Delay(0.6f, [&] {
                OnLeave();
                }).Delay(0.3f, [&] {
                    ui.RequestPopPanel();
                    });
        }
    }

    void PlayerDataPanel::Update(f32 d_t, Engine::Core::Context& ctx)
    {
        for (const auto& opt : opts) {
            opt->Update(d_t, ctx);
        }
        UpdateAnimations(d_t);
    }

    void PlayerDataPanel::UpdateAnimations(f32 d_t)
    {
        if (is_init) {
            if (alpha < 1.0f) {
                alpha += d_t * 2.0f;
                if (alpha > 1.0f) alpha = 1.0f;
            }
        }
        else {
            if (alpha > 0.0f) {
                alpha -= d_t * 3.0f;
                if (alpha < 0.0f) alpha = 0.0f;
            }
        }
        for (const auto& info_bcg : info_bcgs) {
            auto* sprite = info_bcg->GetComponent<Engine::Core::Components::SpriteComponent>();
            if (sprite) {
                sprite->SetColorMod({ 255, 255, 255, static_cast<u8>(alpha * 204) });
            }
        }
    }

    void PlayerDataPanel::Render(Engine::Core::Context& ctx)
    {
        for (const auto& b : info_bcgs) {
            b->Render(ctx);
        }
        for (const auto& opt : opts) {
            opt->Render(ctx);
        }

        auto& text = ui.GetTextManager();
        SDL_Color title_col = info.title_config.color;
        title_col.a = static_cast<u8>(alpha * 255);
        text.RenderText(info.title, Engine::Maths::Vec2{ 640.0f, 100.0f },
            title_col, "hwzs", info.title_config.font_size,
            info.title_config.line_spacing, Engine::Maths::Align::CENTER);
        SDL_Color content_col = info.content_config.color;
        content_col.a = static_cast<u8>(alpha * 255);

        float start_y = 200.0f;
        float line_h = info.content_config.font_size + 20.0f;
        float left_margin = 250.0f;

        for (int i = 0; i < rankings.size(); ++i) {
            const auto& e = rankings[i];

            std::string rank_str = std::to_string(i + 1);
            std::stringstream score_ss;
            score_ss << std::setw(10) << std::setfill('0') << e.score;

            float cur_x = left_margin;
            float cur_y = start_y + i * line_h;
            text.RenderText(rank_str, { cur_x, cur_y }, content_col, "msyh",
                info.content_config.font_size, 0.0f, Engine::Maths::Align::CENTER_LEFT);
            cur_x += 50.0f;
            text.RenderText(e.name, { cur_x, cur_y }, content_col, "msyh",
                info.content_config.font_size, 0.0f, Engine::Maths::Align::CENTER_LEFT);
            cur_x += 200.0f;
            text.RenderText(score_ss.str(), { cur_x, cur_y }, content_col, "msyh",
                info.content_config.font_size, 0.0f, Engine::Maths::Align::CENTER_LEFT);
            cur_x += 250.0f;
            text.RenderText(e.stage, { cur_x, cur_y }, content_col, "msyh",
                info.content_config.font_size, 0.0f, Engine::Maths::Align::CENTER_LEFT);
            cur_x += 150.0f;
            text.RenderText(e.date, { cur_x, cur_y }, content_col, "msyh",
                info.content_config.font_size, 0.0f, Engine::Maths::Align::CENTER_LEFT);
        }

        if (rankings.empty()) {
            text.RenderText("NO DATA", { 640.0f, 400.0f }, content_col, "msyh",
                30.0f, 0.0f, Engine::Maths::Align::CENTER);
        }
    }

    void PlayerDataPanel::OnEnter()
    {
         ui.GetAudioManager().PlayBGM("playerdata");
    }

    void PlayerDataPanel::OnActivate()
    {
    }

    void PlayerDataPanel::OnLeave()
    {
        for (const auto& opt : opts) {
            Engine::Render::Effect::EffectManager::SlideLeftOut(opt.get(), 0.5f);
        }
    }
}