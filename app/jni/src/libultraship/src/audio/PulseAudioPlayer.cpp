#if false //defined(__linux__) || defined(__BSD__)

#include "PulseAudioPlayer.h"
#include "Context.h"
#include <spdlog/spdlog.h>

namespace LUS {
static void PasContextStateCb(pa_context* c, void* userData) {
    switch (pa_context_get_state(c)) {
        case PA_CONTEXT_READY:
        case PA_CONTEXT_TERMINATED:
        case PA_CONTEXT_FAILED:
            *(bool*)userData = true;
            break;
        default:
            break;
    }
}

static void PasStreamStateCb(pa_stream* s, void* userData) {
    switch (pa_stream_get_state(s)) {
        case PA_STREAM_READY:
        case PA_STREAM_FAILED:
        case PA_STREAM_TERMINATED:
            *(bool*)userData = true;
            break;
        default:
            break;
    }
}

static void PasStreamWriteCb(pa_stream* s, size_t length, void* userData) {
}

static void PasUpdateComplete(pa_stream* stream, int success, void* userData) {
    *(bool*)userData = true;
}

static void PasWriteComplete(void* userData) {
    *(bool*)userData = true;
}

PulseAudioPlayer::PulseAudioPlayer() : AudioPlayer() {
}

bool PulseAudioPlayer::DoInit() {
    bool done = false;
    const pa_buffer_attr* appliedAttr = nullptr;

    // Create mainloop
    mMainLoop = pa_mainloop_new();
    if (mMainLoop == NULL) {
        return false;
    }

    // Create context and connect
    mContext =
        pa_context_new(pa_mainloop_get_api(mMainLoop), ("libultraship - " + Context::GetInstance()->GetName()).c_str());
    if (mContext == NULL) {
        goto fail;
    }

    pa_context_set_state_callback(mContext, PasContextStateCb, &done);

    if (pa_context_connect(mContext, NULL, PA_CONTEXT_NOFLAGS, NULL) < 0) {
        goto fail;
    }

    while (!done) {
        pa_mainloop_iterate(mMainLoop, true, NULL);
    }
    pa_context_set_state_callback(mContext, NULL, NULL);
    if (pa_context_get_state(mContext) != PA_CONTEXT_READY) {
        goto fail;
    }

    // Create stream
    pa_sample_spec ss;
    ss.format = PA_SAMPLE_S16LE;
    ss.rate = this->GetSampleRate();
    ss.channels = 2;

#define SAMPLES_HIGH 752
#define SAMPLES_LOW 720

    pa_buffer_attr attr;
    // set the max length to the desired buffered level, plus
    // 3x the high sample rate, which is what the n64 audio engine
    // can output at one time, x2 to avoid overflow in case of the
    // n64 audio engine running faster than pulseaudio, all multiplied
    // by 4 because each sample is 4 bytes
    attr.maxlength = (GetDesiredBuffered() + 3 * SAMPLES_HIGH * 2) * 4;

    // slightly more than one double audio update
    attr.prebuf = SAMPLES_HIGH * 3 * 1.5 * 4;

    attr.minreq = 222 * 4;
    attr.tlength = (GetSampleRate() / 20) * 4;

    // initialize to a value that is deemed sensible by the server
    attr.fragsize = (uint32_t)-1;

    mStream = pa_stream_new(mContext, "zelda", &ss, NULL);
    if (mStream == NULL) {
        goto fail;
    }

    done = false;
    pa_stream_set_state_callback(mStream, PasStreamStateCb, &done);
    pa_stream_set_write_callback(mStream, PasStreamWriteCb, NULL);
    if (pa_stream_connect_playback(mStream, NULL, &attr, PA_STREAM_ADJUST_LATENCY, NULL, NULL) < 0) {
        goto fail;
    }

    while (!done) {
        pa_mainloop_iterate(mMainLoop, true, NULL);
    }
    pa_stream_set_state_callback(mStream, NULL, NULL);
    if (pa_stream_get_state(mStream) != PA_STREAM_READY) {
        goto fail;
    }

    appliedAttr = pa_stream_get_buffer_attr(mStream);
    SPDLOG_TRACE("maxlength: {}\ntlength: {}\nprebuf: {}\nminreq: {}\nfragsize: {}\n", appliedAttr->maxlength,
                 appliedAttr->tlength, appliedAttr->prebuf, appliedAttr->minreq, appliedAttr->fragsize);
    mAttr = *appliedAttr;

    return true;

fail:
    if (mStream != NULL) {
        pa_stream_unref(mStream);
        mStream = NULL;
    }
    if (mContext != NULL) {
        pa_context_disconnect(mContext);
        pa_context_unref(mContext);
        mContext = NULL;
    }
    if (mMainLoop != NULL) {
        pa_mainloop_free(mMainLoop);
        mMainLoop = NULL;
    }

    SPDLOG_ERROR("Failed to initialize PulseAudio stream!");
    return false;
}

int PulseAudioPlayer::Buffered() {
    if (mStream == NULL) {
        return 0;
    }

    bool done = false;
    pa_stream_update_timing_info(mStream, PasUpdateComplete, &done);
    while (!done) {
        pa_mainloop_iterate(mMainLoop, true, NULL);
    }

    const pa_timing_info* info = pa_stream_get_timing_info(mStream);
    if (info == NULL) {
        SPDLOG_ERROR("pa_stream_get_timing_info failed, state is %d\n", pa_stream_get_state(mStream));
    }
    return (info->write_index - info->read_index) / 4;
}

int PulseAudioPlayer::GetDesiredBuffered() {
    return 2480;
}

void PulseAudioPlayer::Play(const uint8_t* buff, size_t len) {
    if (mStream == NULL || mContext == NULL || mMainLoop == NULL) {
        return;
    }

    size_t ws = mAttr.maxlength - Buffered() * 4;
    if (ws < len) {
        len = ws;
    }
    if (pa_stream_write_ext_free(mStream, buff, len, PasWriteComplete, &mWriteComplete, 0LL, PA_SEEK_RELATIVE) < 0) {
        SPDLOG_ERROR("pa_stream_write failed");
        return;
    }
    while (!mWriteComplete) {
        pa_mainloop_iterate(mMainLoop, true, NULL);
    }
    mWriteComplete = false;
}
} // namespace LUS

#endif
