#pragma once

#ifdef _WIN32

#include "AudioPlayer.h"
#include <wrl/client.h>
#include <mmdeviceapi.h>
#include <audioclient.h>

using namespace Microsoft::WRL;

namespace LUS {
class WasapiAudioPlayer : public AudioPlayer, public IMMNotificationClient {
  public:
    WasapiAudioPlayer();

    int Buffered(void);
    int GetDesiredBuffered(void);
    void Play(const uint8_t* buf, size_t len);

  protected:
    virtual HRESULT STDMETHODCALLTYPE OnDeviceStateChanged(LPCWSTR pwstrDeviceId, DWORD dwNewState);
    virtual HRESULT STDMETHODCALLTYPE OnDeviceAdded(LPCWSTR pwstrDeviceId);
    virtual HRESULT STDMETHODCALLTYPE OnDeviceRemoved(LPCWSTR pwstrDeviceId);
    virtual HRESULT STDMETHODCALLTYPE OnDefaultDeviceChanged(EDataFlow flow, ERole role, LPCWSTR pwstrDefaultDeviceId);
    virtual HRESULT STDMETHODCALLTYPE OnPropertyValueChanged(LPCWSTR pwstrDeviceId, const PROPERTYKEY key);
    virtual ULONG STDMETHODCALLTYPE AddRef();
    virtual ULONG STDMETHODCALLTYPE Release();
    virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, VOID** ppvInterface);
    void ThrowIfFailed(HRESULT res);
    bool SetupStream(void);
    bool DoInit(void);

  private:
    ComPtr<IMMDeviceEnumerator> mDeviceEnumerator;
    ComPtr<IMMDevice> mDevice;
    ComPtr<IAudioClient> mClient;
    ComPtr<IAudioRenderClient> mRenderClient;
    LONG mRefCount;
    UINT32 mBufferFrameCount;
    bool mInitialized;
    bool mStarted;
};
} // namespace LUS
#endif
