#include "AudioManager.h"
#include "context.h"
namespace Engine::Audio {
    using namespace Engine::Resource;
	AudioManager::AudioManager(Context& _ctx)
		:ctx(_ctx), bgm_sound(1.0f), sfx_sound(0.0f)
	{
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

        
        bgm = TrackPtr(MIX_CreateTrack(mixer));
        if (!bgm.get()) {
            return;
        }
	}

    AudioManager::~AudioManager()
    {
        try {
            if (mixer) {
                MIX_DestroyMixer(mixer);
                spdlog::debug("音频混合器销毁");
            }
            MIX_Quit();
        }
        catch (const std::exception& e) {
            spdlog::error("无法释放音频管理器");
        }
    }

    void AudioManager::PlayBGM()
    {
        if (!bgm) {
            spdlog::error("未初始化的bgm轨道");
            return;
        }
        if (IsBGMPlaying()) {
            return;
        }

        if (MIX_GetTrackAudio(bgm.get()) != NULL ||
            MIX_GetTrackAudioStream(bgm.get()) != NULL) {
            MIX_ResumeTrack(bgm.get());
        }
        else {
            spdlog::error("你不能播放一个空BGM轨道");
        }
    }

    void AudioManager::PlayBGM(const std::string& sound_name)
    {
        MIX_SetTrackAudio(bgm.get(), ctx.GetResourceManager().GetAudio(sound_name)->audio.get());
        bool ok = MIX_PlayTrack(bgm.get(), -1);
        if (!ok) {
            spdlog::error("无法播放音频{}", SDL_GetError());
            return;
        }
    }

    void AudioManager::PauseBGM()
    {
        if (MIX_TrackPaused(bgm.get())) {
            return;
        }

        MIX_PauseTrack(bgm.get());
    }

    void AudioManager::StopBGM()
    {
        if (MIX_TrackPaused(bgm.get())) {
            return;
        }

        MIX_SetTrackAudio(bgm.get(), NULL);
        MIX_SetTrackAudioStream(bgm.get(), NULL);
        MIX_SetTrackIOStream(bgm.get(), NULL, false);
    }

    bool AudioManager::IsBGMPlaying() const
    {
        if (MIX_TrackPlaying(bgm.get())) {
            return true;
        }

        return false;
    }

    void AudioManager::AddTrack_SFX(const std::string& track_name)
    {
        auto temp = TrackPtr(MIX_CreateTrack(mixer));
        if (!temp.get()) {
            spdlog::error("不能够添加音频轨道");
        }
        MIX_SetTrackGain(temp.get(), sfx_sound);
        sfx.insert({ track_name, std::move(temp) });
    }

    void AudioManager::PlaySFX(const std::string& sound_name, const std::string& track_name)
    {
        if (sfx.find(track_name) != sfx.end()) {
            TrackPtr& t = sfx[track_name];
            MIX_SetTrackAudio(t.get(), ctx.GetResourceManager().GetAudio(sound_name)->audio.get());
            bool ok = MIX_PlayTrack(t.get(), 0);
            if (!ok) {
                spdlog::error("播放音频：{} 失败", sound_name);
            }
        }
        else {
            AddTrack_SFX(track_name);
            TrackPtr& t = sfx[track_name];
            MIX_SetTrackAudio(t.get(), ctx.GetResourceManager().GetAudio(sound_name)->audio.get());
            bool ok = MIX_PlayTrack(t.get(), 0);
            if (!ok) {
                spdlog::error("播放音频：{} 失败", sound_name);
            }
        }
    }



    void AudioManager::SetSound(float volume)
    {
        if (!mixer) {
            spdlog::error("尝试修改未初始化的mixer");
            return;
        }
        MIX_SetMasterGain(mixer, volume);
    }

    void AudioManager::SetBGMSound(float volume)
    {
        bgm_sound = volume;
        if (!bgm.get()) {
            spdlog::error("未初始化的BGM轨道");
            return;
        }
        MIX_SetTrackGain(bgm.get(), bgm_sound);
    }

    void AudioManager::SetSFXSound(float volume)
    {
        sfx_sound = volume;
        if (sfx.size() == 0) {
            spdlog::error("还未有音效轨道，无法设置");
            return;
        }
        for (auto& [_name, _track] : sfx) {
            MIX_SetTrackGain(_track.get(), sfx_sound);
        }
        
    }

    void AudioManager::SetSFXSound(float volume, const std::string& track_name)
    {
        if (sfx.size() == 0) {
            spdlog::error("还未有音效轨道，无法设置");
            return;
        }

        if (sfx.find(track_name) != sfx.end()) {
            MIX_SetTrackGain(sfx[track_name].get(), volume);
        }
        else {
            spdlog::error("找不到该轨道：{}", track_name);
        }
    }

}