#include "OverPanel.h"
#include "../../../Engine/InputManager.h"
#include "../../../Engine/AudioManager.h"
#include "../../../Engine/schedule.h"
#include "../../../Engine/UIManager.h"
#include "../../../Engine/EffectManager.h"
#include "../../../Engine/Components/TransformComponent.h"
#include "../../../Engine/Components/SpriteComponent.h"
#include "../../../Engine/Components/AnimationComponent.h"
#include "../../../Engine/text.h" 
#include "../TitleScene.h"
#include "../../GameData.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <algorithm>

namespace Game::UI {
    const std::vector<std::string> KEYBOARD_ROWS = {
        "ABCDEFGHIJKLM",
        "NOPQRSTUVWXYZ",
        "abcdefghijklm",
        "nopqrstuvwxyz",
        "0123456789+-=",
        ".,!?@:;[]()_/"
    };

    OverPanel::OverPanel(const std::string& ui_name, Engine::UISystem::UIManager& _ui)
        : Engine::UISystem::Panel(ui_name), ui(_ui)
    {
    }

    void OverPanel::Init(Engine::Core::Context& ctx) {
        InitMenuComponents(ctx);
        LoadScores();

        ScoreEntry new_entry;
        new_entry.score = static_cast<uint64_t>(gamedata.score);
        std::string date_str = GetCurrentDate();
        strncpy_s(new_entry.date, date_str.c_str(), sizeof(new_entry.date) - 1);
        strncpy_s(new_entry.stage, "stage", sizeof(new_entry.stage) - 1);
        memset(new_entry.name, 0, sizeof(new_entry.name));

        rankings.push_back(new_entry);

        current_rank_index = static_cast<int>(rankings.size() - 1);
        current_state = State::NAME_INPUT;
    }

    void OverPanel::InitMenuComponents(Engine::Core::Context& ctx) {
        std::ifstream file("data/pause_panel.json");
        nlohmann::json json;
        if (file.is_open()) {
            file >> json;
        }
        else { 
            spdlog::error("data/pause_panel.json 加载失败"); 
            return; 
        }

        std::vector<std::vector<std::unique_ptr<Engine::Core::GameObject>>> vec1(3);
        std::vector<std::vector<std::unique_ptr<Engine::Core::GameObject>>> vec2(3);
        if (json.contains("over_panel")) {
            for (const auto& c : json["over_panel"]) {
                std::unique_ptr<Engine::Core::GameObject> gb =
                    std::make_unique<Engine::Core::GameObject>(c["id"].get<std::string>());

                gb->AddComponent<Engine::Core::Components::TransformComponent>(
                    Engine::Maths::Vec2{ c["position"][0], c["position"][1] },
                    Engine::Maths::Vec2{ c["scale"][0], c["scale"][1] });

                gb->AddComponent<Engine::Core::Components::SpriteComponent>(
                    c["source"], ctx.GetResourceManager(),
                    SDL_FRect{ c["rect"][0], c["rect"][1], c["rect"][2], c["rect"][3] },
                    Engine::Maths::Align::CENTER_LEFT);

                if (c.count("transparent")) {
                    gb->GetComponent<Engine::Core::Components::SpriteComponent>()->SetColorMod(SDL_Color{
                        255, 255, 255, static_cast<uint8_t>(255 * c["transparent"].get<float>()) });
                }

                int layer_idx = c.at("layer").get<int32_t>();
                if (layer_idx < 3) {
                    if (c["type"] == "bcg") {
                        vec2[layer_idx].push_back(std::move(gb));
                    }
                    else if (c["type"] == "opt") {
                        vec1[layer_idx].push_back(std::move(gb));
                    }
                }
            }
        }
        opts = std::move(vec1);
        bcgs = std::move(vec2);
        is_menu_init = true;
    }

    void OverPanel::HandleInput(Engine::Core::Context& ctx, Engine::Audio::AudioManager& au, Engine::Scene::SceneManager& sm) {
        if (current_state == State::NAME_INPUT) {
            auto& input = ctx.GetInputManager();
            if (input.IsActionPressed("move_forward")) {
                au.PlaySFX("se_select00", "select");
                cursor_row--;
                if (cursor_row < 0) cursor_row = 6;
            }
            if (input.IsActionPressed("move_back")) {
                au.PlaySFX("se_select00", "select");
                cursor_row++;
                if (cursor_row > 6) cursor_row = 0;
            }

            int max_col = 12;
            if (input.IsActionPressed("move_left")) {
                au.PlaySFX("se_select00", "select");
                cursor_col--;
                if (cursor_col < 0) cursor_col = max_col;
            }
            if (input.IsActionPressed("move_right")) {
                au.PlaySFX("se_select00", "select");
                cursor_col++;
                if (cursor_col > max_col) cursor_col = 0;
            }
            if (input.IsActionPressed("esc")) {
                au.PlaySFX("se_cancel00", "select");
                current_state = State::MENU;
                menu_index = 0;
                menu_layer = 1;
                if (!opts[menu_layer].empty()) {
                    Engine::Render::Effect::EffectManager::BreatheColor(
                        opts[menu_layer][menu_index].get(), SDL_Color{ 255, 0, 0, 255 }
                    );
                }
            }
            if (input.IsActionPressed("ok")) {
                au.PlaySFX("se_ok00", "select");

                if (cursor_row < 6) {
                    if (current_input_name.length() < max_name_length) {
                        current_input_name += KEYBOARD_ROWS[cursor_row][cursor_col];
                    }
                }
                else {
                    if (cursor_col < 10) {
                        std::string syms = "{}|~^#$%&*";
                        if (current_input_name.length() < max_name_length) current_input_name += syms[cursor_col];
                    }
                    else if (cursor_col == 10) { // 空格
                        if (current_input_name.length() < max_name_length) current_input_name += ' ';
                    }
                    else if (cursor_col == 11) { // BS
                        if (!current_input_name.empty()) current_input_name.pop_back();
                    }
                    else if (cursor_col == 12) {
                        FinishInput(ctx);
                    }
                }
            }
        }
        else {
            HandleMenuInput(ctx, au, sm);
        }
    }

    void OverPanel::FinishInput(Engine::Core::Context& ctx) {
        if (current_rank_index != -1) {
            std::string final_name = current_input_name.empty() ? "NO NAME" : current_input_name;
            strncpy_s(rankings[current_rank_index].name, final_name.c_str(), sizeof(rankings[current_rank_index].name) - 1);
            std::sort(rankings.begin(), rankings.end(), [](const ScoreEntry& a, const ScoreEntry& b) {
                return a.score > b.score;
                });

            SaveScores();
        }
        current_state = State::MENU;
        menu_index = 0;
        menu_layer = 1;
        if (!opts[menu_layer].empty()) {
            Engine::Render::Effect::EffectManager::BreatheColor(
                opts[menu_layer][menu_index].get(), SDL_Color{ 255, 0, 0, 255 });
        }
    }

    void OverPanel::HandleMenuInput(Engine::Core::Context& ctx, Engine::Audio::AudioManager& au, 
        Engine::Scene::SceneManager& sm) {
        if (!is_menu_init) {
            return;
        }

        auto& input = ctx.GetInputManager();

        if (input.IsActionPressed("move_forward")) {
            --menu_index;
        }
        else if (input.IsActionPressed("move_back")) {
            ++menu_index;
        }

        if (opts[menu_layer].empty()) {
            return;
        }

        if (menu_index < 0) {
            menu_index = static_cast<int16_t>(opts[menu_layer].size() - 1);
        }
        else if (menu_index >= opts[menu_layer].size()) {
            menu_index = 0;
        }

        if (menu_old_index != menu_index) {
            if (menu_old_index != -1 && menu_old_index < opts[menu_layer].size()) {
                au.PlaySFX("se_select00", "select");
                Engine::Render::Effect::EffectManager::StopBreatheColor(opts[menu_layer][menu_old_index].get());
            }
            Engine::Render::Effect::EffectManager::BreatheColor(
                opts[menu_layer][menu_index].get(), SDL_Color{ 255, 0, 0, 255 });
        }
        if (input.IsActionPressed("ok")) {
            au.PlaySFX("se_ok00", "select");
            if (menu_layer == 1) {
                OptChooseLayer1(ctx, au, sm);
            }
            else {
                OptChooseLayer2(ctx, au, sm);
            }
        }

        menu_old_index = menu_index;
    }

    void OverPanel::OptChooseLayer1(Engine::Core::Context& ctx, Engine::Audio::AudioManager& au, Engine::Scene::SceneManager& sm) {
        switch (menu_index) {
        case 0:
            Engine::Render::Effect::EffectManager::StopBreatheColor(opts[menu_layer][menu_index].get());
            menu_layer = 2;
            menu_index = 1;
            opt_type = OptType::LEAVE;
            break;
        case 1:
            Engine::Render::Effect::EffectManager::StopBreatheColor(opts[menu_layer][menu_index].get());
            menu_layer = 2;
            menu_index = 1;
            opt_type = OptType::RESTART;
            break;
        }
        if (!opts[menu_layer].empty())
            Engine::Render::Effect::EffectManager::BreatheColor(opts[menu_layer][menu_index].get(), SDL_Color{ 255, 0, 0, 255 });
    }

    void OverPanel::OptChooseLayer2(Engine::Core::Context& ctx, Engine::Audio::AudioManager& au, Engine::Scene::SceneManager& sm) {
        switch (menu_index) {
        case 0: {
            if (opt_type == OptType::LEAVE) {
                sm.RequestReplaceScene(std::make_unique<Game::Scene::TitleScene>("title", ctx, sm, au, ui));
                ui.Clear();
            }
            else if (opt_type == OptType::RESTART) {
                sm.GetCurrentScene()->Reset();
            }
        }break;
        case 1:
        default: {
            Engine::Render::Effect::EffectManager::StopBreatheColor(opts[menu_layer][menu_index].get());
            menu_layer = 1;
            menu_index = static_cast<int16_t>(opt_type == OptType::RESTART ? 1 : 0);
            Engine::Render::Effect::EffectManager::BreatheColor(opts[menu_layer][menu_index].get(), SDL_Color{ 255, 0, 0, 255 });
        }break;
        }
    }

    void OverPanel::Update(float d_t, Engine::Core::Context& ctx) {
        if (current_state == State::NAME_INPUT) {
            blink_timer += d_t;
            if (blink_timer >= 0.5f) {
                show_cursor = !show_cursor;
                blink_timer = 0.0f;
            }
            if (current_rank_index != -1) {
                std::string disp = current_input_name;
                if (show_cursor && disp.length() < max_name_length) disp += "_";
                strncpy_s(rankings[current_rank_index].name, disp.c_str(), sizeof(rankings[current_rank_index].name) - 1);
            }
        }
        else {
            for (int i = 0; i <= menu_layer; ++i) {
                if (i < bcgs.size()) for (auto& bcg : bcgs[i]) bcg->Update(d_t, ctx);
                if (i < opts.size()) for (auto& opt : opts[i]) opt->Update(d_t, ctx);
            }
        }
    }

    void OverPanel::Render(Engine::Core::Context& ctx) {
        if (current_state == State::NAME_INPUT) {
            if (is_menu_init && !bcgs[0].empty()) {
                for (auto& bg : bcgs[0]) {
                    if (bg->GetName() == "baku") {
                        continue;
                    }
                    bg->Render(ctx);
                }
            }

            auto& tm = ui.GetTextManager();
            float center_x = 487.5f;
            float left_margin = 100.0f;
            tm.RenderText("Score Ranking!!", { center_x, 80.0f }, 
                { 255, 255, 255, 255 }, "msyh", 32.0f, 0.0f, Engine::Maths::Align::CENTER);
            float start_y = 150.0f;
            float line_h = 32.0f;

            for (int i = 0; i < rankings.size(); ++i) {
                const auto& e = rankings[i];
                SDL_Color col = { 150, 150, 150, 255 };

                if (i == current_rank_index) {
                    col = { 255, 255, 0, 255 };
                }
                else if (i < 3) {
                    col = { 255, 200, 200, 255 };
                }
                if (i == 10) {
                    col = { 200, 50, 50, 255 };
                }

                std::string rank_str = std::to_string(i + 1);
                std::stringstream score_ss;
                score_ss << std::setw(10) << std::setfill('0') << e.score;
                float cur_x = left_margin;
                tm.RenderText(rank_str, { cur_x, start_y + i * line_h }, col, "msyh", 
                    22.0f, 0.0f, Engine::Maths::Align::CENTER_LEFT);
                cur_x += 50.0f;
                tm.RenderText(e.name, { cur_x, start_y + i * line_h }, col, "msyh", 
                    22.0f, 0.0f, Engine::Maths::Align::CENTER_LEFT);
                cur_x += 200.0f;
                tm.RenderText(score_ss.str(), { cur_x, start_y + i * line_h }, col, "msyh", 
                    22.0f, 0.0f, Engine::Maths::Align::CENTER_LEFT);
                cur_x += 180.0f;
                tm.RenderText(e.stage, { cur_x, start_y + i * line_h }, col, "msyh", 
                    22.0f, 0.0f, Engine::Maths::Align::CENTER_LEFT);
                cur_x += 150.0f;
                tm.RenderText(e.date, { cur_x, start_y + i * line_h }, col, "msyh", 
                    22.0f, 0.0f, Engine::Maths::Align::CENTER_LEFT);
            }

            if (current_rank_index != -1) {
                float kb_start_y = 550.0f;
                float kb_x = 150.0f;
                float w = 45.0f;
                float h = 45.0f;

                for (int r = 0; r < 6; ++r) {
                    for (int c = 0; c < KEYBOARD_ROWS[r].size(); ++c) {
                        SDL_Color k_col = { 100, 100, 100, 255 };
                        if (r == cursor_row && c == cursor_col) k_col = { 255, 50, 50, 255 };
                        std::string s(1, KEYBOARD_ROWS[r][c]);
                        tm.RenderText(s, { kb_x + c * w, kb_start_y + r * h }, k_col, "msyh", 28.0f, 0.0f, Engine::Maths::Align::CENTER);
                    }
                }

                float y_last = kb_start_y + 6 * h;
                float cur_x = kb_x;
                std::string syms = "{}|~^#$%&*";
                for (int c = 0; c < 13; ++c) {
                    SDL_Color k_col = { 100, 100, 100, 255 };
                    if (cursor_row == 6 && cursor_col == c) k_col = { 255, 50, 50, 255 };

                    if (c < 10) {
                        std::string s(1, syms[c]);
                        tm.RenderText(s, { cur_x, y_last }, k_col, "msyh", 28.0f, 0.0f, Engine::Maths::Align::CENTER);
                        cur_x += w;
                    }
                    else if (c == 10) {
                        tm.RenderText("□", { cur_x, y_last }, k_col, "msyh", 28.0f, 0.0f, Engine::Maths::Align::CENTER);
                        cur_x += w;
                    }
                    else if (c == 11) {
                        tm.RenderText("BS", { cur_x + 10.0f, y_last }, k_col, "msyh", 24.0f, 0.0f, Engine::Maths::Align::CENTER);
                        cur_x += w * 1.5f;
                    }
                    else if (c == 12) {
                        tm.RenderText("终", { cur_x + 10.0f, y_last }, k_col, "msyh", 28.0f, 0.0f, Engine::Maths::Align::CENTER);
                    }
                }
            }
        }
        else {
            for (int i = 0; i <= menu_layer; ++i) {
                if (i < bcgs.size()) {
                    for (auto& bcg : bcgs[i]) bcg->Render(ctx);
                }
                if (i < opts.size()) {
                    for (auto& opt : opts[i]) opt->Render(ctx);
                }
            }
        }
    }

    void OverPanel::LoadScores() {
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

    void OverPanel::SaveScores() {
        std::ofstream out("data/score.dat", std::ios::binary | std::ios::trunc);
        if (out.is_open()) {
            int count = 0;
            for (const auto& e : rankings) {
                out.write(reinterpret_cast<const char*>(&e), sizeof(ScoreEntry));

                count++;
                if (count >= MAX_RANKINGS) {
                    break;
                }
            }
            out.close();
        }
    }

    std::string OverPanel::GetCurrentDate() {
        std::time_t t = std::time(nullptr);
        std::tm tm_buf;
        localtime_s(&tm_buf, &t);
        std::stringstream ss;
        ss << std::put_time(&tm_buf, "%y/%m/%d");
        return ss.str();
    }

    void OverPanel::OnEnter() {
        ui.GetAudioManager().PlayBGM("gameover");
        if (is_menu_init) {
            for (const auto& layer : bcgs) {
                for (const auto& obj : layer) {
                    if (obj->GetName() == "black_blur") {
                        Engine::Render::Effect::EffectManager::FadeIn(obj.get(), 0.5f, 0, 204);
                    }
                    else {
                        Engine::Render::Effect::EffectManager::FadeIn(obj.get(), 0.2f);
                        if (obj->GetName() == "baku") {
                            Engine::Render::Effect::EffectManager::Swing(obj.get(), 20.0f, 0.3f);
                        }
                    }
                }
            }
        }
    }
}