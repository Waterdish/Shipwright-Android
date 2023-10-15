#include "Audio.h"

#include "Context.h"

namespace LUS {
Audio::Audio() {
}

Audio::~Audio() {
    SPDLOG_TRACE("destruct audio");
}

void Audio::InitAudioPlayer() {
    switch (GetAudioBackend()) {
#ifdef _WIN32
        case AudioBackend::WASAPI:
            mAudioPlayer = std::make_shared<WasapiAudioPlayer>();
            break;
#endif
#if false//defined(__linux)
        case AudioBackend::PULSE:
            mAudioPlayer = std::make_shared<PulseAudioPlayer>();
            break;
#endif
        default:
            mAudioPlayer = std::make_shared<SDLAudioPlayer>();
    }

    if (mAudioPlayer) {
        if (!mAudioPlayer->Init()) {
            // Failed to initialize system audio player.
            // Fallback to SDL if the native system player does not work.
            SetAudioBackend(AudioBackend::SDL);
            mAudioPlayer = std::make_shared<SDLAudioPlayer>();
            mAudioPlayer->Init();
        }
    }
}

void Audio::Init() {
    mAvailableAudioBackends = std::make_shared<std::vector<AudioBackend>>();
#ifdef _WIN32
    mAvailableAudioBackends->push_back(AudioBackend::WASAPI);
#endif
#ifdef __linux
    mAvailableAudioBackends->push_back(AudioBackend::PULSE);
#endif
    mAvailableAudioBackends->push_back(AudioBackend::SDL);

    SetAudioBackend(Context::GetInstance()->GetConfig()->GetAudioBackend());
}

std::shared_ptr<AudioPlayer> Audio::GetAudioPlayer() {
    return mAudioPlayer;
}

AudioBackend Audio::GetAudioBackend() {
    return mAudioBackend;
}

void Audio::SetAudioBackend(AudioBackend backend) {
    mAudioBackend = backend;
    Context::GetInstance()->GetConfig()->SetAudioBackend(GetAudioBackend());
    Context::GetInstance()->GetConfig()->Save();

    InitAudioPlayer();
}

std::shared_ptr<std::vector<AudioBackend>> Audio::GetAvailableAudioBackends() {
    return mAvailableAudioBackends;
}

} // namespace LUS
