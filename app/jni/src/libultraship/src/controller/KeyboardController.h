#pragma once
#include "Controller.h"
#include <string>

namespace LUS {
class KeyboardController : public Controller {
  public:
    KeyboardController(int32_t deviceIndex);

    void ReadDevice(int32_t portIndex) override;
    const std::string GetButtonName(int32_t portIndex, int32_t n64bitmask) override;
    bool PressButton(int32_t scancode);
    bool ReleaseButton(int32_t scancode);
    bool Connected() const override;
    bool CanRumble() const override;
    bool CanSetLed() const override;
    bool CanGyro() const override;
    void ClearRawPress() override;
    int32_t SetRumble(int32_t portIndex, bool rumble) override;
    int32_t SetLedColor(int32_t portIndex, Color_RGB8 color) override;
    void CreateDefaultBinding(int32_t portIndex) override;
    int32_t ReadRawPress() override;
    void ReleaseAllButtons();
    void SetLastScancode(int32_t key);
    int32_t GetLastScancode();

  protected:
    int32_t mLastScancode;
    int32_t mLastKey = -1;
};
} // namespace LUS
