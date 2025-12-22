#pragma once
#include "../prefix.h"
#include "maths.h"
#include FT_FREETYPE_H
struct SDL_Color;
namespace Engine::Core {
    class Context;
}
namespace Engine::Maths {
    struct Vec2;
}
namespace Engine::Render {
    struct ASCIIConfig {
        char character;
        float x;
        float y;
        float width;
        float height;
        float x_offset;
        float y_offset;
    };

    class TextManager {
    private:
        std::unordered_map<char, ASCIIConfig> ascii;
        Core::Context& ctx;
        FT_Library ft = nullptr;

    public:
        TextManager(Core::Context& _ctx, const std::string& _config_path);
        ~TextManager();

    public:
        void RenderText(
            const std::string& text, const Maths::Vec2& pos,
            SDL_Color color, const std::string& name,
            float font_size, float spacing, 
            Engine::Maths::Align align = Engine::Maths::Align::CENTER_RIGHT);
        void RenderTextASCII(const std::string& str,
            const Maths::Vec2& pos, const Maths::Vec2& scale,
            float spacing, Engine::Maths::Align align = Engine::Maths::Align::CENTER_RIGHT);
        const FT_Library GetFTLib() const { return ft; }
    };
}