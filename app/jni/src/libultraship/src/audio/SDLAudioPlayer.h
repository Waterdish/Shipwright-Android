#pragma once
#include "AudioPlayer.h"
#include <SDL2/SDL.h>

namespace LUS {
class SDLAudioPlayer : public AudioPlayer {
  public:
    SDLAudioPlayer();

    int Buffered(void);
    int GetDesiredBuffered(void);
    void Play(const uint8_t* buf, size_t len);

  protected:
    bool DoInit(void);

  private:
    SDL_AudioDeviceID mDevice;
};
} // namespace LUS
