cmake_minimum_required (VERSION 3.15)

if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project (worky VERSION 1.0.0)


find_package(SDL3 REQUIRED)
find_package(SDL3_image REQUIRED)
find_package(SDL3_mixer REQUIRED)
find_package(assimp REQUIRED)
find_package(Freetype REQUIRED)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR})

add_compile_options(/W4)

# 将源代码添加到此项目的可执行文件。
add_executable (worky WIN32
main.cpp
"Engine/engine.h" 
"Engine/engine.cpp"   
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
"Engine/collider.cpp" 
"Engine/Components/EffectComponent.h" 
"Engine/maths.cpp"
"Engine/SceneManager.h" 
"Engine/SceneManager.cpp" 
"Engine/Components/States/PlayerState.h" 
"Engine/Components/PlayerComponent.h" 
"Engine/Components/PlayerComponent.cpp" 
"Engine/AudioManager.h" 
"Engine/AudioManager.cpp" 
"Engine/InputManager.h" 
"Engine/InputManager.cpp"  
"Engine/animation.h" 
"Engine/animation.cpp" 
"Engine/sprite.h" 
"Engine/Components/AnimationComponent.h" 
"Engine/Components/AnimationComponent.cpp"  
"Engine/Components/States/PlayerState.cpp" 
"Engine/effects.h" 
"Engine/EffectManager.h"   
"Engine/ui.h" 
"Engine/UIManager.h" 
"Engine/UIManager.cpp"
"Engine/schedule.h" 
"Engine/D3D/model.h"  
"Engine/text.h" 
"Engine/text.cpp"
"Engine/camera.h" 
"Engine/camera.cpp" 
"Engine/CameraManager.h" 
"Engine/CameraManager.cpp" 
"Game/Player/Idel.h" 
"Game/Player/Idel.cpp" 
"Game/Player/Move.h" 
"Game/Player/Move.cpp" 
"Game/StateMachine.h"
"Game/Item/Item.h"
"Game/GameData.h" 
"Game/Player/Player.h" 
"Game/Player/Player.cpp"
"Game/Game.h" 
"Game/Game.cpp" 
"Game/Enemy/Enemy.h" 
"Game/Enemy/Enemy.cpp" 
"Game/Bullet/Bullet.h" 
"Game/Bullet/Bullet.h"
"Game/Bullet/BulletManager.h" 
"Game/Bullet/BulletManager.cpp" 
"Game/Scene/TitleScene.h" 
"Game/Scene/TitleScene.cpp" 
"Game/Scene/Stage.h" 
"Game/Scene/Stage.cpp" 
"Game/Scene/Loading.h" 
"Game/Scene/Loading.cpp" 
"Game/Scene/UIPanel/MainPanel.h" 
"Game/Scene/UIPanel/MainPanel.cpp" 
"Game/Scene/UIPanel/OptionPanel.h" 
"Game/Scene/UIPanel/OptionPanel.cpp"  
"Game/Scene/UIPanel/InformationPanel.h" 
"Game/Scene/UIPanel/InformationPanel.cpp"     
"Game/Scene/UIPanel/PausePanel.h" 
"Game/Scene/UIPanel/PausePanel.cpp" 
"Game/Scene/UIPanel/ScorePanel.h" 
"Game/Scene/UIPanel/ScorePanel.cpp" 
"Game/Enemy/EnemyManager.h" 
"Game/Enemy/EnemyManager.cpp" 
"Game/Enemy/EnemyComponent.h" 
"Game/Enemy/EnemyComponent.cpp" 
"Game/Player/Players/Reimu.h" 
"Game/Player/Players/Reimu.cpp" 
"Game/Player/PlayerBulletManager.h" 
"Game/Player/PlayerBulletManager.cpp" 
"Game/Item/ItemManager.h" 
"Game/Item/ItemManager.cpp" 
"Game/Scene/UIPanel/OverPanel.h" 
"Game/Scene/UIPanel/OverPanel.cpp"   
"Game/Scene/UIPanel/PlayerDataPanel.h" 
"Game/Scene/UIPanel/PlayerDataPanel.cpp" 
"Game/Script/ScriptLevel.h" 
"Game/Script/ScriptLevel.cpp" 
"Game/Script/ScriptBackGround.h" 
"Game/Script/ScriptBackGround.cpp")
target_include_directories(worky PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_compile_definitions(worky PRIVATE 
    $<$<CONFIG:Release>:SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_OFF>
)

file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/worky.rc
     "IDI_ICON1 ICON DISCARDABLE \"${CMAKE_SOURCE_DIR}/favicon.ico\"\n")
target_sources(worky PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/worky.rc)
target_link_libraries(
	worky 
	SDL3::SDL3
	SDL3_image::SDL3_image
	SDL3_mixer::SDL3_mixer
	assimp::assimp
	d3d11
    dxgi
    d3dcompiler
	Freetype::Freetype
    optimized "${CMAKE_SOURCE_DIR}/include/lib/freetype.lib"

    debug "${CMAKE_SOURCE_DIR}/include/lua/Debug/lua.lib"
    optimized "${CMAKE_SOURCE_DIR}/include/lua/Release/lua.lib"
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})
if(MSVC)
    target_compile_options(worky PRIVATE /Zi /Od)
    target_compile_definitions(worky PRIVATE D3D11_DEBUG)

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
