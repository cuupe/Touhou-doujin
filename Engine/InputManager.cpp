#include "InputManager.h"

namespace Engine::Input {
	InputManager::InputManager(SDL_Renderer* sdl_renderer)
		:renderer(sdl_renderer)
	{
        if (!renderer) {
            spdlog::error("输入管理器无法获取SDL_Renderer");
            throw std::runtime_error("输入管理器: SDL_Renderer 为空指针");
        }
        actions_to_keyname_map.clear();
        scancode_to_actions_map.clear();


        //test 硬编码
        {
            actions_to_keyname_map.insert({ "move_forward",
                std::vector<std::string>{
                "UP"
            } });
            actions_to_keyname_map.insert({ "move_back",
                std::vector<std::string>{
                "DOWN"
            } });
            actions_to_keyname_map.insert({ "move_left",
                std::vector<std::string>{
                "LEFT"
            } });
            actions_to_keyname_map.insert({ "move_right",
                std::vector<std::string>{
                "RIGHT"
            } });
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