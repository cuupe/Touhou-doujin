#include "text.h"
#include "render.h"
#include "context.h"
#include "D3D/model.h"
#include "resources.h"
#include "maths.h"
#include <fstream>
#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>
namespace Engine::Render {
    TextManager::TextManager(Core::Context& _ctx, const std::string& _config_path)
        :ctx(_ctx)
    {
        using json = nlohmann::json;

        std::ifstream file(_config_path);
        if (!file.is_open()) {
            spdlog::error("ASCII配置加载错误");
            return;
        }

        json jsonData;
        file >> jsonData;

        std::string source = jsonData["source"];
        const auto& asciiChars = jsonData["ascii_chars"];

        for (const auto& entry : asciiChars) {
            ASCIIConfig info;
            info.character = entry["char"].get<std::string>()[0];
            info.x = entry["x"].get<float>();
            info.y = entry["y"].get<float>();
            info.width = entry["width"].get<float>();
            info.height = entry["height"].get<float>();
            info.x_offset = entry["x_offset"].get<float>();
            info.y_offset = entry["y_offset"].get<float>();

            ascii[info.character] = info;
        }

        if (FT_Init_FreeType(&ft)) {
            spdlog::error("FreeType: 加载FT库失败");
            ft = nullptr;
        }
    }

    TextManager::~TextManager()
    {
        if (ft) {
            FT_Done_FreeType(ft);
            ft = nullptr;
        }
    }

    static uint32_t GetNextUTF8Char(const std::string& str, size_t& index) {
        if (index >= str.size()) return 0;

        unsigned char c = static_cast<unsigned char>(str[index]);
        uint32_t codepoint = 0;
        int bytes = 0;

        if (c < 0x80) {
            codepoint = c;
            bytes = 1;
        }
        else if ((c & 0xE0) == 0xC0) {
            codepoint = c & 0x1F;
            bytes = 2;
        }
        else if ((c & 0xF0) == 0xE0) {
            codepoint = c & 0x0F;
            bytes = 3;
        }
        else if ((c & 0xF8) == 0xF0) {
            codepoint = c & 0x07;
            bytes = 4;
        }
        else {
            index++;
            return '?';
        }

        for (int i = 1; i < bytes; ++i) {
            if (index + i >= str.size()) break;
            unsigned char next = static_cast<unsigned char>(str[index + i]);
            codepoint = (codepoint << 6) | (next & 0x3F);
        }

        index += bytes;
        return codepoint;
    }

    void TextManager::RenderText(const std::string& text, const Maths::Vec2& pos,
        SDL_Color color, const std::string& font_name, float font_size, float spacing, Engine::Maths::Align align)
    {
        auto& res = ctx.GetResourceManager();
        auto& renderer = ctx.GetRenderer();
        auto* device_ctx = ctx.GetDeviceContext();
        auto* font_res = res.GetFont(font_name);
        if (!font_res) {
            return;
        }

        float scale = font_size / font_res->loaded_size;
        float total_width = 0.0f;
        float total_height = font_size;

        if (align != Engine::Maths::Align::TOP_LEFT && align != Engine::Maths::Align::NONE) {
            size_t temp_idx = 0;
            while (temp_idx < text.size()) {
                uint32_t codepoint = GetNextUTF8Char(text, temp_idx);
                if (codepoint == 0) break;

                const auto* glyph = res.CacheGlyph(device_ctx, font_name, (char32_t)codepoint);
                if (!glyph) continue;

                total_width += (glyph->advance * scale) + spacing;
            }
            if (total_width > 0) total_width -= spacing;
        }

        float pen_x = pos.x;
        float pen_y = pos.y;

        switch (align) {
        case Engine::Maths::Align::TOP_CENTER:
        case Engine::Maths::Align::CENTER:
        case Engine::Maths::Align::BOTTOM_CENTER:
            pen_x -= total_width / 2.0f;
            break;
        case Engine::Maths::Align::TOP_RIGHT:
        case Engine::Maths::Align::CENTER_RIGHT:
        case Engine::Maths::Align::BOTTOM_RIGHT:
            pen_x -= total_width;
            break;
        default: break;
        }

        switch (align) {
        case Engine::Maths::Align::CENTER_LEFT:
        case Engine::Maths::Align::CENTER:
        case Engine::Maths::Align::CENTER_RIGHT:
            pen_y -= total_height / 2.0f;
            break;
        case Engine::Maths::Align::BOTTOM_LEFT:
        case Engine::Maths::Align::BOTTOM_CENTER:
        case Engine::Maths::Align::BOTTOM_RIGHT:
            pen_y -= total_height;
            break;
        default: break;
        }

        std::vector<D3D::Vertex> vertices;
        vertices.reserve(text.size() * 6);
        DirectX::XMFLOAT4 col = {
            color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f
        };

        size_t index = 0;
        while (index < text.size()) {
            uint32_t codepoint = GetNextUTF8Char(text, index);
            if (codepoint == 0) {
                break;
            }

            const auto* glyph = res.CacheGlyph(device_ctx, font_name, (char32_t)codepoint);
            if (!glyph) {
                continue;
            }

            float w = glyph->width * scale;
            float h = glyph->height * scale;
            float x_off_scaled = glyph->x_offset * scale;
            float y_off_scaled = glyph->y_offset * scale;
            float xpos = pen_x + x_off_scaled;
            float ypos = pen_y + (font_size - y_off_scaled);

            vertices.push_back({ {xpos, ypos, 0},     {glyph->u0, glyph->v0}, col });
            vertices.push_back({ {xpos + w, ypos, 0}, {glyph->u1, glyph->v0}, col });
            vertices.push_back({ {xpos + w, ypos + h, 0}, {glyph->u1, glyph->v1}, col });
            vertices.push_back({ {xpos + w, ypos + h, 0}, {glyph->u1, glyph->v1}, col });
            vertices.push_back({ {xpos, ypos + h, 0},     {glyph->u0, glyph->v1}, col });
            vertices.push_back({ {xpos, ypos, 0},         {glyph->u0, glyph->v0}, col });

            pen_x += (glyph->advance * scale) + spacing;
        }

        if (!vertices.empty()) {
            renderer.DrawVertices(
                font_res->atlas_srv.Get(),
                vertices);
        }
    }

    void TextManager::RenderTextASCII(const std::string& str,
        const Engine::Maths::Vec2& pos, const Engine::Maths::Vec2& scale, float spacing, Engine::Maths::Align align)
    {
        auto& r = ctx.GetRenderer();
        auto& res = ctx.GetResourceManager();
        const Resource::TextureResource* texture = res.GetTexture("ascii");
        if (!texture) {
            spdlog::error("未找到ascii的资源");
            return;
        }
        float total_width = 0.0f;
        float max_height = 0.0f;

        if (align != Engine::Maths::Align::TOP_LEFT && align != Engine::Maths::Align::NONE) {
            for (char c : str) {
                auto it = ascii.find(c);
                if (it == ascii.end()) {
                    total_width += 10.0f * scale.x;
                }
                else {
                    const ASCIIConfig& info = it->second;
                    total_width += info.width * scale.x;
                    float h = info.height * scale.y;
                    if (h > max_height) max_height = h;
                }
                total_width += spacing;
            }
            if (!str.empty() && total_width > 0) {
                total_width -= spacing;
            }
        }
        float currentX = pos.x;
        float currentY = pos.y;

        switch (align) {
        case Engine::Maths::Align::TOP_CENTER:
        case Engine::Maths::Align::CENTER:
        case Engine::Maths::Align::BOTTOM_CENTER:
            currentX -= total_width / 2.0f;
            break;
        case Engine::Maths::Align::TOP_RIGHT:
        case Engine::Maths::Align::CENTER_RIGHT:
        case Engine::Maths::Align::BOTTOM_RIGHT:
            currentX -= total_width;
            break;
        default: break;
        }

        switch (align) {
        case Engine::Maths::Align::CENTER_LEFT:
        case Engine::Maths::Align::CENTER:
        case Engine::Maths::Align::CENTER_RIGHT:
            currentY -= max_height / 2.0f;
            break;
        case Engine::Maths::Align::BOTTOM_LEFT:
        case Engine::Maths::Align::BOTTOM_CENTER:
        case Engine::Maths::Align::BOTTOM_RIGHT:
            currentY -= max_height;
            break;
        default: break;
        }

        for (char c : str) {
            auto it = ascii.find(c);
            if (it == ascii.end()) {
                currentX += 10.0f * scale.x;
                continue;
            }

            const ASCIIConfig& info = it->second;
            SDL_FRect srcRect = {
                info.x, info.y, info.width, info.height
            };
            SDL_FRect dstRect = {
                currentX + info.x_offset * scale.x,
                currentY + info.y_offset * scale.y,
                info.width * scale.x,
                info.height * scale.y
            };
            r.Draw(
                texture,
                &srcRect,
                &dstRect,
                0.0,
                nullptr,
                nullptr,
                0
            );
            currentX += info.width * scale.x + spacing;
        }
    }

}