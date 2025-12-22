#pragma once
//SDL3
#include <SDL3/SDL.h>
#include <SDL3/SDL_properties.h>
#include <SDL3/SDL_render.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_mixer/SDL_mixer.h>
//STL
#include <memory>
#include <iostream>
#include <string>
#include <vector>
#include <codecvt>
#include <locale>
#include <fstream>
#include <filesystem>
#include <algorithm>
#include <map>
#include <unordered_map>
#include <typeindex>
#include <type_traits>
#include <utility> 
#include <optional>
#include <cmath>
#include <random>

//其他
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <wrl/client.h>
#include <dxgi1_4.h>
#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
extern "C" {
#include <lua/lua.h>
#include <lua/lualib.h>
#include <lua/lauxlib.h>
}
#ifdef max
#undef max
#endif

#ifdef min
#undef min
#endif




