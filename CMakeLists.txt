cmake_minimum_required (VERSION 3.8)

if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project (worky VERSION 0.1.0)


find_package(SDL3 REQUIRED)
find_package(SDL3_image REQUIRED)
find_package(SDL3_mixer REQUIRED)
find_package(SDL3_ttf REQUIRED)
find_package(assimp REQUIRED)
find_package(DirectXTex CONFIG REQUIRED)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR})

add_compile_options(/W4)

# 将源代码添加到此项目的可执行文件。
add_executable (worky 
main.cpp 
"Engine/engine.h" 
"Engine/engine.cpp"   
"Engine/graphapi.h" 
"Engine/graphapi.cpp" 
"Engine/model.h" 
"Engine/model.cpp" 
"Engine/time.h" 
"Engine/time.cpp" 
"Engine/resources.h" 
"Engine/resources.cpp" 
"Engine/GameObject.h" 
"Engine/GameObject.cpp"
"Engine/component.h" 
"Engine/render.h" 
"Engine/render.cpp" 
"Engine/context.h" 
"Engine/context.cpp" 
"Engine/Components/TransformComponent.h" 
"Engine/Components/TransformComponent.cpp" 
"Engine/maths.h" 
"Engine/Components/SpriteComponent.h"
"Engine/Components/SpriteComponent.cpp" 
"Engine/scene.h" 
"Engine/scene.cpp"
"Engine/Components/ColliderComponent.h" 
"Engine/Components/ColliderComponent.cpp" 
"Engine/collider.h"  
"Engine/maths.cpp"
"Engine/SceneManager.h" 
"Engine/SceneManager.cpp" 
"Game/Player/Player.h" 
"Game/Game.h" 
"Game/Game.cpp" 
"Game/Enemy/Enemy.h" 
"Game/Player/Player.cpp" 
"Game/Enemy/Enemy.cpp" 
"Game/GameScene.h" 
"Game/GameScene.cpp" 
 "Game/Bullet/Bullet.h" 
 "Game/Bullet/Bullet.cpp" 
 "Game/Player/Skill.h" 
 "Game/Player/Skill.cpp" 
 "Engine/Components/States/PlayerState.h" 
 "Engine/Components/PlayerComponent.h" 
 "Engine/Components/PlayerComponent.cpp" 
 "Game/Player/Idel.h" 
 "Game/Player/Slow.h" 
 "Game/Player/Idel.cpp" 
 "Game/Player/Slow.cpp" 
 "Game/Player/Move.h" 
 "Game/Player/Move.cpp" 
 "Engine/InputManager.h" 
 "Engine/InputManager.cpp"  
 "Game/StateMachine.h" 
 "Engine/config.h" 
 "Engine/config.cpp" 
 "Engine/animation.h" 
 "Engine/animation.cpp" 
 "Engine/Components/AnimationComponent.h" 
 "Engine/Components/AnimationComponent.cpp"  
 "Engine/Components/States/PlayerState.cpp" 
 "Game/Item.h" 
 "Game/Item.cpp" 
 "Game/GameData.h" 
 "Engine/AudioManager.h" 
 "Engine/AudioManager.cpp" 
 "Game/Bullet/BulletComponent.h" 
 "Game/Bullet/BulletComponent.cpp" 
 "Engine/sprite.h"   
 "Game/Bullet/BulletManager.h" 
 "Game/Bullet/BulletManager.cpp" 
 "Game/Scene/UI.h" 
 "Game/Scene/UI.cpp" 
 "Game/Scene/Stage.h" 
 "Game/Scene/Stage.cpp")
target_include_directories(worky PRIVATE ${PROJECT_SOURCE_DIR}/include)

target_link_libraries(
	worky 
	SDL3::SDL3
	SDL3_image::SDL3_image
	SDL3_mixer::SDL3_mixer
    SDL3_ttf::SDL3_ttf
	assimp::assimp
	d3d11
    dxgi
    d3dcompiler
	Microsoft::DirectXTex
)


if(MSVC)
    target_compile_options(worky PRIVATE /Zi /Od)  # /Zi: 生成 PDB, /Od: 禁用优化 (Debug)
    target_compile_definitions(worky PRIVATE D3D11_DEBUG)  # 启用 D3D11 调试层 (加载符号)

    # Post-build: 复制 PDB 到输出目录 (VS 调试需 PDB)
    add_custom_command(TARGET worky POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_PDB_FILE:worky> $<TARGET_FILE_DIR:worky>
    )
endif()
if(MSVC)
    target_compile_options(worky PRIVATE /utf-8)
    target_compile_definitions(worky PRIVATE _UTF8)
endif()

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET worky PROPERTY CXX_STANDARD 20)
endif()

# TODO: 如有需要，请添加测试并安装目标。
