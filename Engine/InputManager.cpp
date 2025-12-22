#include "InputManager.h"
#include "render.h"
#include "../prefix.h"
namespace Engine::Input {
    using json = nlohmann::json;
	InputManager::InputManager(Render::Renderer* _renderer)
		:render(_renderer)
	{
        if (!render) {
            spdlog::error("输入管理器无法获取Render");
            throw std::runtime_error("输入管理器: Renderer 为空指针");
        }
        actions_to_keyname_map.clear();
        scancode_to_actions_map.clear();
        try {


            std::ifstream f("data/input_config.json");
            json config;
            f >> config;

            for (const auto& i : config.at("input_config")) {
                std::vector<std::string> str;
                for (const auto& j : i.at("code")) {
                    str.emplace_back(j.get<std::string>());
                }
                actions_to_keyname_map.insert({
                    i.at("name").get<std::string>(),
                    std::move(str)
                    });
            }

            for (const auto& pair : actions_to_keyname_map) {
                action_states[pair.first] = ActionState::INACTIVE;

                for (const std::string& n : pair.second) {
                    SDL_Scancode s = GetScancodeByString(n);
                    if (s != SDL_SCANCODE_UNKNOWN) {
                        scancode_to_actions_map[s].push_back(pair.first);
                    }
                    else {
                        spdlog::error("无法映射按键消息");
                    }
                }
            }
        }
        catch (const std::exception& err) {
            spdlog::error("发生错误");
            return;
        }
    }

    bool InputManager::IsActionDown(const std::string & action_name) const
    {
        if (auto it = action_states.find(action_name); it != action_states.end()) {
            return it->second == ActionState::PRESSED_THIS_FRAME || it->second
                == ActionState::HELD_DOWN;
        }
        return false;
    }

    bool InputManager::IsActionPressed(const std::string& action_name) const
    {
        if (auto it = action_states.find(action_name); it != action_states.end()) {
            return it->second == ActionState::PRESSED_THIS_FRAME;
        }
        return false;
    }

    bool InputManager::IsActionReleased(const std::string& action_name) const
    {
        if (auto it = action_states.find(action_name); it != action_states.end()) {
            return it->second == ActionState::RELEASED_THIS_FRAME;
        }
        return false;
    }

    bool InputManager::ShouldQuit() const
    {
        return quit;
    }

    void InputManager::ResetAllState()
    {
        for (auto& pair : action_states) {
            pair.second = ActionState::INACTIVE;
        }
    }

    void InputManager::SetShouldQuit(bool should_quit)
    {
        quit = should_quit;
    }

    void InputManager::ProcessEvent(const SDL_Event& event){
        switch (event.type) {
        case SDL_EVENT_WINDOW_MINIMIZED:
            break;
        case SDL_EVENT_WINDOW_RESTORED:
            break;
        case SDL_EVENT_WINDOW_RESIZED: {
        }break;   
        case SDL_EVENT_KEY_DOWN:
        case SDL_EVENT_KEY_UP: {
            SDL_Scancode sc = event.key.scancode;
            bool is_down = event.key.down;
            bool is_repeat = event.key.repeat;

            auto it = scancode_to_actions_map.find(sc);
            if (it != scancode_to_actions_map.end()) {
                const std::vector<std::string>& associated_actions = it->second;
                for (const std::string& a_n : associated_actions) {
                    UpdateActionState(a_n, is_down, is_repeat);
                }
            }
        }break;
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
        case SDL_EVENT_MOUSE_BUTTON_UP: {
            //需要鼠标时再添加
        }break;
        case SDL_EVENT_QUIT:
            quit = true;
            break;
        default:break;
        }
    }

    void InputManager::UpdateActionState(const std::string& action_name, bool is_input_active, bool is_repeat_event)
    {
        auto it = action_states.find(action_name);
        if (it == action_states.end()) {
            spdlog::error("尝试更新未注册的动作状态: {}", action_name);
            return;
        }

        if (is_input_active) {
            if (is_repeat_event) {
                it->second = ActionState::HELD_DOWN;
            }
            else {
                it->second = ActionState::PRESSED_THIS_FRAME;
            }
        }
        else {
            it->second = ActionState::RELEASED_THIS_FRAME;
        }
    }

    SDL_Scancode InputManager::GetScancodeByString(const std::string& action_name)
    {
        return SDL_GetScancodeFromName(action_name.c_str());
    }

	void InputManager::Update() { 
        for (auto& pair : action_states) {
            if (pair.second == ActionState::PRESSED_THIS_FRAME) {
                pair.second = ActionState::HELD_DOWN;
            }
            else if (pair.second == ActionState::RELEASED_THIS_FRAME) {
                pair.second = ActionState::INACTIVE;
            }
        }

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            ProcessEvent(event);
        }
    }
}