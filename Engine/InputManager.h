#pragma once
#include "../prefix.h"
#include "../Engine/maths.h"
namespace Engine::Render {
	class Renderer;
}

namespace Engine::Input {
	enum class ActionState {
		INACTIVE,
		PRESSED_THIS_FRAME,
		HELD_DOWN,
		RELEASED_THIS_FRAME
	};
	class InputManager final {
	private:
		Render::Renderer* render;
		std::unordered_map<std::string, std::vector<std::string>> actions_to_keyname_map;
		std::unordered_map<SDL_Scancode, std::vector<std::string>> scancode_to_actions_map;
		bool quit = false;
		
		std::unordered_map<std::string, ActionState> action_states;
	public:
		InputManager(Render::Renderer* _renderer);

	public:
		bool IsActionDown(const std::string& action_name) const;
		bool IsActionPressed(const std::string& action_name) const;
		bool IsActionReleased(const std::string& action_name) const;

		bool ShouldQuit() const;
		void SetShouldQuit(bool should_quit);

	private:
		void ProcessEvent(const SDL_Event& event);
		void UpdateActionState(const std::string& action_name,
			bool is_input_active, bool is_repeat_event);
		SDL_Scancode GetScancodeByString(const std::string& action_name);

	public:
		void Update();

	};
}

#include <SDL3/SDL_scancode.h>