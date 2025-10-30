#pragma once
#include "../prefix.h"

namespace Resources {
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


    std::unique_ptr<TextureResource> LoadTexture(SDL_Renderer* renderer, const std::string& path);
    //加载文件夹下所有2D纹理
    void LoadTextures(SDL_Renderer* renderer, const std::string& path, std::map<std::string, std::unique_ptr<TextureResource>>& texs);
    //加载音频资源
    std::unique_ptr<AudioResource> LoadAudio(MIX_Mixer* mixer, const std::string& path, bool predecode = false);
    //加载文件夹下所有音频资源
    void LoadAudios(MIX_Mixer* mixer, const std::string& path, std::map<std::string, std::unique_ptr<AudioResource>>& audios, bool predecode = false);
    //加载字体资源
    std::unique_ptr<FontResource> LoadFont(SDL_Renderer* renderer, const std::string& path, int size);
    //加载文件夹下所有字体资源
    void LoadFonts(SDL_Renderer* renderer, const std::string& path, int size, std::map<std::string, std::unique_ptr<FontResource>>& fonts);

    //TODO: directx模型加载（ASSIMP）
    //static void LoadModels(const std::string& path);
}