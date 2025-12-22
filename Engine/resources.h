#pragma once
#include "../prefix.h"

namespace Engine::Render::D3D {
    struct Vertex;
    struct Vertex1;
    struct Mesh;
    struct ModelResource;
    using ModelPtr = std::shared_ptr<ModelResource>;
};
namespace Engine::Render {
    class Renderer;
}
namespace Engine::Resource {
    using Microsoft::WRL::ComPtr;
    struct SDLTextureDeleter {
        void operator()(SDL_Texture* t) const {
            if (t) {
                SDL_DestroyTexture(t);
            }
        }
    };
    struct TextureResource {
        std::unique_ptr<SDL_Texture, SDLTextureDeleter> texture = nullptr;
        ComPtr<ID3D11ShaderResourceView> d3d_srv;
        int width = 0;
        int height = 0;
    };
    using TexturePtr = std::unique_ptr<SDL_Texture, SDLTextureDeleter>;

    // MIX_Audio 是静态音频数据，每一个音频对应一个实例
    struct MixAudioDeleter {
        void operator()(MIX_Audio* a) const {
            if (a) {
                MIX_DestroyAudio(a);
            }
        }
    };
    struct AudioResource {
        std::unique_ptr<MIX_Audio, MixAudioDeleter> audio = nullptr;
    };
    using MixAudioPtr = std::unique_ptr<MIX_Audio, MixAudioDeleter>;

    struct MIXTrackDeleter {
        void operator()(MIX_Track* t) const {
            if (t) {
                MIX_DestroyTrack(t);
            }
        }
    };
    using TrackPtr = std::unique_ptr<MIX_Track, MIXTrackDeleter>;

    struct glyph_data {
        float u0, v0, u1, v1;
        float width, height;
        float x_offset, y_offset;
        float advance;
    };

    struct FontResource {
        FT_Face face = nullptr;
        ComPtr<ID3D11Texture2D> atlas_texture;
        ComPtr<ID3D11ShaderResourceView> atlas_srv;
        int atlas_width = 1024;
        int atlas_height = 1024;
        int current_x = 0;
        int current_y = 0;
        int max_row_height = 0;
        int loaded_size = 0;

        std::unordered_map<char32_t, glyph_data> glyphs;

        bool try_pack(int w, int h, int& out_x, int& out_y) {
            int padding = 1;

            if (current_x + w + padding >= atlas_width) {
                current_x = 0;
                current_y += max_row_height + padding;
                max_row_height = 0;
            }

            if (current_y + h + padding >= atlas_height) {
                return false;
            }

            out_x = current_x;
            out_y = current_y;

            current_x += w + padding;
            if (h > max_row_height) max_row_height = h;

            return true;
        }
    };


    class ResourceManager final{
    private:
        std::map<std::string, std::unique_ptr<AudioResource>> audios;
        std::map<std::string, std::unique_ptr<TextureResource>> texs;
        std::map<std::string, std::unique_ptr<FontResource>> fonts;
        ComPtr<ID3D11ShaderResourceView> default_srv;
        std::map<std::string, Render::D3D::ModelPtr> models;

        
    public:
        ResourceManager(ID3D11Device* device);

    public:
        void LoadTexture(SDL_Renderer* renderer, ID3D11Device* device, const std::string& path);
        void LoadAudio(MIX_Mixer* mixer, const std::string& path, bool predecode = false);
        void LoadFont(ID3D11Device* device, FT_Library ft, const std::string& path, int size);
        Render::D3D::ModelPtr LoadModel(SDL_Renderer* render, ID3D11Device* device, const std::string& path);
        ID3D11ShaderResourceView* GetDefaultSRV() { return default_srv.Get(); }
    public:
        AudioResource* GetAudio(const std::string& name) {
            if (audios.find(name) != audios.end()) {
                return audios[name].get();
            }
            else {
                spdlog::error("找不到音频资源：{}", name);
                return nullptr;
            }
        }

        TextureResource* GetTexture(const std::string& name) {
            if (texs.find(name) != texs.end()) {
                return texs[name].get();
            }
            else {
                spdlog::error("找不到图片资源：{}", name);
                return nullptr;
            }
        }

        FontResource* GetFont(const std::string& name) {
            if (fonts.find(name) != fonts.end()) {
                return fonts[name].get();
            }
            else {
                spdlog::error("找不到字体资源：{}", name);
                return nullptr;
            }
        }
        const glyph_data* CacheGlyph(ID3D11DeviceContext* ctx, const std::string& font_name, char32_t code_point);

        Render::D3D::ModelResource* GetModel(const std::string& name) {
            if (models.find(name) != models.end()) {
                return models[name].get();
            }
            else {
                spdlog::error("找不到模型资源: {}", name);
                return nullptr;
            }
        }
    };
}