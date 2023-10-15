#pragma once
#include "controller/Controller.h"
#include <string>

// since clang-tidy doesn't build Wii U, this file is not found
// this error is benign, so we're supressing it
// NOLINTNEXTLINE
#include <padscore/wpad.h>

namespace LUS {
class WiiUController : public Controller {
  public:
    WiiUController(int32_t deviceIndex, WPADChan chan);
    bool Open();
    void Close();
    void ReadDevice(int32_t portIndex) override;
    bool Connected() const;
    bool CanGyro() const;
    bool CanRumble() const;
    bool CanSetLed() const override;
    void ClearRawPress() override;
    int32_t ReadRawPress() override;
    int32_t SetRumble(int32_t portIndex, bool rumble) override;
    int32_t SetLedColor(int32_t portIndex, Color_RGB8 color) override;
    const std::string GetButtonName(int32_t portIndex, int32_t n64bitmask) override;

  protected:
    void CreateDefaultBinding(int32_t portIndex) override;

  private:
    std::string GetControllerExtensionName();

    bool mConnected;
    WPADChan mChan;
    WPADExtensionType mExtensionType;
};
} // namespace LUS
