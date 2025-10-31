#include "engine.h"

namespace Engine {
    engine::engine(const char* win_name, int width, int height, int flag, int fps)
        :t(fps), event(), width(width), height(height)
    {
        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
            spdlog::error("SDL 初始化失败: {}", SDL_GetError());
            return;
        }
        TTF_Init();
        spdlog::info("SDL 初始化成功");

        if (!MIX_Init()) {
            spdlog::error("SDL_mixer 初始化失败: {}", SDL_GetError());
            SDL_Quit();
            return;
        }

        window = SDL_CreateWindow(win_name, width, height, flag);
        if (!window) {
            spdlog::error("窗口创建失败: {}", SDL_GetError());
            SDL_Quit();
            return;
        }
        spdlog::info("窗口创建成功");


        renderer = SDL_CreateRenderer(window, "direct3d11");
        if (!renderer) {
            spdlog::error("D3D11 Renderer 创建失败: {}", SDL_GetError());
            SDL_DestroyWindow(window);
            SDL_Quit();
            return;
        }
        SDL_SetRenderLogicalPresentation(renderer, width, height, SDL_LOGICAL_PRESENTATION_LETTERBOX);
        spdlog::info("D3D11 Renderer 创建成功");


        SDL_AudioSpec spec;
        spec.freq = 22050;
        spec.format = SDL_AUDIO_S16;
        spec.channels = 2;

        mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec);
        if (!mixer) {
            spdlog::error("打开音频失败:{}", SDL_GetError());
            MIX_Quit();
            SDL_Quit();
            return;
        }


        SDL_PropertiesID props = SDL_GetRendererProperties(renderer);
        if (props == 0) {
            std::cout << "获取 Properties 失败" << std::endl;
            return;
        }

        void* devicePtr = SDL_GetPointerProperty(props, SDL_PROP_RENDERER_D3D11_DEVICE_POINTER, nullptr);

        if (devicePtr) {
            ID3D11Device* rawDevice = static_cast<ID3D11Device*>(devicePtr);
            if (rawDevice) {
                ULONG refCount = rawDevice->AddRef();
                spdlog::debug("AddRef 返回引用计数: {}", refCount);

                device = ComPtr<ID3D11Device>(rawDevice);
                if (!device) {
                    spdlog::error("ComPtr<ID3D11Device> 构造失败 (ptr 有效但 ComPtr 为空)");
                    rawDevice->Release();
                    return;
                }
                spdlog::debug("ComPtr<ID3D11Device> 构造成功，引用计数: {}", device->AddRef());

                device->GetImmediateContext(context.GetAddressOf());
                if (!context) {
                    spdlog::error("GetImmediateContext 失败 (device 有效但 context 为空)");
                    return;
                }
                spdlog::info("D3D11 device/context 获取成功");
            }
            else {
                spdlog::error("static_cast<ID3D11Device*> 失败 (devicePtr 无效类型)");
                return;
            }
        }
        else {
            spdlog::error("D3D11 devicePtr 为空，创建失败");
            return;
        }

        spdlog::info("D3D11 设备/上下文获取成功（已准备好使用）");
        initialized = true;

    }

    engine::~engine()
    {
        try {
            if (renderer) {
                SDL_DestroyRenderer(renderer);
                spdlog::debug("Renderer 销毁");
            }
            if (window) {
                SDL_DestroyWindow(window);
                spdlog::debug("窗口销毁");
            }
            if (mixer) {
                MIX_DestroyMixer(mixer);
                spdlog::debug("音频混合器销毁");
            }
            device.Reset();
            context.Reset();
            TTF_Quit();
            MIX_Quit();
            SDL_Quit();
        }
        catch (const std::exception& e) {
            spdlog::error("Engine 清理异常: {}", e.what());
        }
        spdlog::info("Engine 清理完成");
    }
}




