#include "resources.h"

//整体加载函数尽量不用
namespace Engine::Resource {
    void ResourceMannager::LoadTexture(SDL_Renderer* renderer, const std::string& path)
    {
        namespace fs = std::filesystem;
        std::filesystem::path fs_path(path);
        std::string filename = fs_path.stem().string();
        if (!renderer) {
            spdlog::error("renderer为空，加载失败");
            return;
        }
        SDL_Texture* texu = IMG_LoadTexture(renderer, path.c_str());
        if (!texu) {
            spdlog::error("加载纹理失败：{} ，SDL_image错误：{}", path.c_str(), SDL_GetError());
            throw std::runtime_error("发生错误");
            return;
        }

        int w, h;
        SDL_PropertiesID props = SDL_GetTextureProperties(texu);
        if (!props) {
            spdlog::error("获取纹理属性失败: {}，SDL错误：{}", path.c_str(), SDL_GetError());
            throw std::runtime_error("发生错误");
            return;
        }

        w = texu->w;
        h = texu->h;
        if (w <= 0 || h <= 0) {
            spdlog::error("提取纹理尺寸失败:w= {}， h= {}", w, h);
            throw std::runtime_error("发生错误");
            return;
        }

        TexturePtr te(texu);
        std::unique_ptr<TextureResource> tex_res = std::make_unique<TextureResource>();
        tex_res->height = h;
        tex_res->width = w;
        tex_res->texture = std::move(te);

        texs.insert({ filename, std::move(tex_res)});
    }

    void ResourceMannager::LoadTextures(SDL_Renderer* renderer,
        const std::string& path) {
        namespace fs = std::filesystem;

        if (!fs::exists(path) || !fs::is_directory(path)) {
            spdlog::error("路径错误 : {}", path);
            throw std::runtime_error("发生错误");
            return;
        }

        for (const auto& entry : fs::directory_iterator(path)) {
            if (entry.is_regular_file()) {
                std::string ext = entry.path().extension().string();
                if (ext == ".png" || ext == ".jpg" || ext == ".jpeg" || ext == ".bmp" || ext == ".gif") {
                    std::string full_path = entry.path().string();
                    SDL_Texture* tex = IMG_LoadTexture(renderer, full_path.c_str());

                    if (tex) {
                        std::string key = entry.path().stem().string();
                        TexturePtr te(tex);
                        std::unique_ptr<TextureResource> tex_res = std::make_unique<TextureResource>();
                        tex_res->height = tex->h;
                        tex_res->width = tex->w;
                        tex_res->texture = std::move(te);
                        texs[key] = std::move(tex_res);
                    }
                    else {
                        spdlog::error("无法加载纹理 :{} ，SDL_ERROR: {}", full_path, SDL_GetError());
                        throw std::runtime_error("发生错误");
                    }
                }
            }
        }
    }

    void ResourceMannager::LoadAudio(MIX_Mixer* mixer, const std::string& path, bool predecode)
    {
        namespace fs = std::filesystem;
        std::filesystem::path fs_path(path);
        std::string filename = fs_path.stem().string();
        MIX_Audio* audio = MIX_LoadAudio(mixer, path.c_str(), predecode);
        if (!audio) {
            spdlog::error("加载音频失败: {}", SDL_GetError());
            throw std::runtime_error("发生错误");
            return;
        }
        std::unique_ptr<AudioResource> au = std::make_unique<AudioResource>();
        au->audio = MixAudioPtr(audio);

        audios.insert({ filename, std::move(au) });
    }


    void ResourceMannager::LoadFont(SDL_Renderer* renderer, const std::string& path, int size)
    {

    }

    void ResourceMannager::LoadFonts(SDL_Renderer* renderer,
        const std::string& path,
        int size)
    {

    }
};

