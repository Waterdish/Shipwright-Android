#pragma once

#if false//defined(__linux__) || defined(__BSD__)

#include "AudioPlayer.h"
#include <pulse/pulseaudio.h>

namespace LUS {
class PulseAudioPlayer : public AudioPlayer {
  public:
    PulseAudioPlayer();

    int Buffered() override;
    int GetDesiredBuffered() override;
    void Play(const uint8_t* buff, size_t len) override;

  protected:
    bool DoInit() override;

  private:
    pa_context* mContext = nullptr;
    pa_stream* mStream = nullptr;
    pa_mainloop* mMainLoop = nullptr;
    bool mWriteComplete = false;
    pa_buffer_attr mAttr = { 0 };
};
} // namespace LUS
#endif
