#pragma once
#include "../prefix.h"
#include "resources.h"
namespace Engine::Core {
	class Context;
}
using namespace Engine::Core;
namespace Engine::Audio {
	class AudioManager final {
	private:
		MIX_Mixer* mixer = nullptr;
		Context& ctx;

	private:
		Engine::Resource::TrackPtr bgm;	//背景音乐
		float bgm_sound;
		std::map<std::string, Engine::Resource::TrackPtr> sfx;	//音频轨道 - 音效
		float sfx_sound;
	
	public:
		explicit AudioManager(Context&);
		~AudioManager();
		AudioManager(const AudioManager&) = delete;
		AudioManager& operator=(const AudioManager&) = delete;
		AudioManager(AudioManager&&) = delete;
		AudioManager& operator=(AudioManager&&) = delete;

	public:
		MIX_Mixer* GetMixer() const { return mixer; }

	public:
		void PlayBGM();
		void PlayBGM(const std::string&);
		void PauseBGM();
		void StopBGM();
		bool IsBGMPlaying() const;
		void AddTrack_SFX(const std::string&);
		void PlaySFX(const std::string&, const std::string&);
		void SetSound(float);
		void SetBGMSound(float);
		void SetSFXSound(float);
		void SetSFXSound(float, const std::string&);

	};
}