#include "resources.h"


std::unique_ptr<Resources::TextureResource> Resources::LoadTexture(SDL_Renderer* renderer, const std::string& path)
{
    if (!renderer) {
        spdlog::error("renderer为空，加载失败");
        return nullptr;
    }
    SDL_Texture* tex = IMG_LoadTexture(renderer, path.c_str());
    if (!tex) {
        spdlog::error("加载纹理失败：{} ，SDL_image错误：{}", path.c_str(), SDL_GetError());
        throw std::runtime_error("发生错误");
        return nullptr;
    }

    int w, h;
    SDL_PropertiesID props = SDL_GetTextureProperties(tex);
    if (!props) {
        spdlog::error("获取纹理属性失败: {}，SDL错误：{}", path.c_str(), SDL_GetError());
        throw std::runtime_error("发生错误");
        return nullptr;
    }

    w = tex->w;
    h = tex->h;
    if (w <= 0 || h <= 0) {
        spdlog::error("提取纹理尺寸失败:w= {}， h= {}", w, h);
        throw std::runtime_error("发生错误");
        return nullptr;
    }

    TexturePtr te(tex);
    std::unique_ptr<TextureResource> tex_res = std::make_unique<TextureResource>();
    tex_res->height = h;
    tex_res->width = w;
    tex_res->texture = std::move(te);

    return tex_res;
}

void Resources::LoadTextures(SDL_Renderer* renderer,
    const std::string& path,
    std::map<std::string, std::unique_ptr<Resources::TextureResource>>& texs) {
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

std::unique_ptr<Resources::AudioResource> Resources::LoadAudio(MIX_Mixer* mixer, const std::string& path, bool predecode)
{
    MIX_Audio* audio = MIX_LoadAudio(mixer, path.c_str(), predecode);
    if (!audio) {
        spdlog::error("加载音频失败: {}", SDL_GetError());
        throw std::runtime_error("发生错误");
        return nullptr;
    }
    std::unique_ptr<Resources::AudioResource> au = std::make_unique<Resources::AudioResource>();
    au->audio = Resources::MixAudioPtr(audio);

    return au;
}

void Resources::LoadAudios(MIX_Mixer* mixer, const std::string& path, std::map<std::string, std::unique_ptr<Resources::AudioResource>>& audios, bool predecode)
{

}


std::unique_ptr<Resources::FontResource> Resources::LoadFont(SDL_Renderer* renderer, const std::string& path, int size)
{
    return nullptr;
}

void Resources::LoadFonts(SDL_Renderer* renderer,
    const std::string& path, 
    int size, 
    std::map<std::string, std::unique_ptr<Resources::FontResource>>& fonts)
{

}


