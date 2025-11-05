#pragma once
#include "../prefix.h"
namespace Engine::Resource {
    struct SDLTextureDeleter {
        void operator()(SDL_Texture* t) const {
            if (t) {
                SDL_DestroyTexture(t);
            }
        }
    };//用于释放SDL纹理资源
    struct TextureResource {
        std::unique_ptr<SDL_Texture, SDLTextureDeleter> texture = nullptr;
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


    struct TTFFontDeleter {
        void operator()(TTF_Font* f) const {
            if (f) {
                TTF_CloseFont(f);
            }
        }
    };
    struct FontResource {
        std::unique_ptr<TTF_Font, TTFFontDeleter> font = nullptr;
        int pointSize = 0;
    };

    using FontPtr = std::unique_ptr<TTF_Font, TTFFontDeleter>;


    class ResourceMannager final{
    private:
        std::map<std::string, std::unique_ptr<AudioResource>> audios;
        std::map<std::string, std::unique_ptr<TextureResource>> texs;
        std::map<std::string, std::unique_ptr<FontResource>> fonts;


    public:
        void LoadTexture(SDL_Renderer* renderer, const std::string& path);
        //加载文件夹下所有2D纹理
        void LoadTextures(SDL_Renderer* renderer, const std::string& path);
        //加载音频资源
        void LoadAudio(MIX_Mixer* mixer, const std::string& path, bool predecode = false);
        //加载字体资源
        void LoadFont(SDL_Renderer* renderer, const std::string& path, int size);
        //加载文件夹下所有字体资源
        void LoadFonts(SDL_Renderer* renderer, const std::string& path, int size);

        //TODO: directx模型加载（ASSIMP）
        //static void LoadModels(const std::string& path);

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
    };
}