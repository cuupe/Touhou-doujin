#include "engine.h"
#include "D3D/model.h"
namespace Engine {
    engine::engine(const char* win_name, const char* icon_path, int width, int height, int flag, int fps)
        : t(fps), event(), initialized(false)
    {
        try {
            if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
                throw std::runtime_error(std::string("SDL 初始化失败: ") + SDL_GetError());
            }
            spdlog::info("SDL 初始化成功");
            if (!MIX_Init()) {
                throw std::runtime_error(std::string("SDL_mixer 初始化失败: ") + SDL_GetError());
            }

            window = SDL_CreateWindow(win_name, width, height, flag);
            if (!window) {
                throw std::runtime_error(std::string("窗口创建失败: ") + SDL_GetError());
            }
            spdlog::info("窗口创建成功");
            renderer = SDL_CreateRenderer(window, "direct3d11");
            if (!renderer) {
                throw std::runtime_error(std::string("D3D11 Renderer 创建失败: ") + SDL_GetError());
            }
            spdlog::info("D3D11 Renderer 创建成功");

            SDL_Surface* icon_surface = IMG_Load(icon_path);
            if (!icon_surface) {
                throw std::runtime_error("加载图标失败: " + std::string(icon_path));
            }
            SDL_SetWindowIcon(window, icon_surface);
            SDL_DestroySurface(icon_surface);

            SDL_PropertiesID props = SDL_GetRendererProperties(renderer);
            if (props == 0) {
                throw std::runtime_error("获取 SDL Renderer Properties 失败");
            }
            void* devicePtr = SDL_GetPointerProperty(props, SDL_PROP_RENDERER_D3D11_DEVICE_POINTER, nullptr);
            if (!devicePtr) {
                throw std::runtime_error("D3D11 devicePtr 为空");
            }

            ID3D11Device* rawDevice = static_cast<ID3D11Device*>(devicePtr);
            if (!rawDevice) {
                throw std::runtime_error("static_cast<ID3D11Device*> 失败");
            }
            device.Attach(rawDevice);

            if (!device) {
                rawDevice->Release();
                throw std::runtime_error("ComPtr<ID3D11Device> 构造失败");
            }
            device->GetImmediateContext(device_context.GetAddressOf());
            if (!device_context) {
                throw std::runtime_error("GetImmediateContext 失败");
            }

            spdlog::info("D3D11 设备/上下文获取成功（已准备好使用）");
            initialized = true;

        }
        catch (const std::exception& e) {
            spdlog::error("Engine 构造异常: {}", e.what());

            device_context.Reset();
            device.Reset();
            if (renderer) {
                SDL_DestroyRenderer(renderer);
                renderer = nullptr;
            }

            if (window) {
                SDL_DestroyWindow(window);
                window = nullptr;
            }
            SDL_Quit();

            initialized = false;
        }
    }

    engine::~engine()
    {
        try {
            device_context.Reset();
            device.Reset();
            if (renderer) {
                SDL_DestroyRenderer(renderer);
                spdlog::debug("Renderer 销毁");
            }
            if (window) {
                SDL_DestroyWindow(window);
                spdlog::debug("窗口销毁");
            }

            SDL_Quit();
        }
        catch (const std::exception& e) {
            spdlog::error("Engine 清理异常: {}", e.what());
        }
        spdlog::info("Engine 清理完成");
    }
}




