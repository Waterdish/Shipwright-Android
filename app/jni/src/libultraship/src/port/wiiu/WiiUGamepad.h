#pragma once
#include "controller/Controller.h"
#include <string>

namespace LUS {
class WiiUGamepad : public Controller {
  public:
    WiiUGamepad(int32_t deviceIndex);

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
    bool mConnected = true;
    float mRumblePatternStrength;
    uint8_t mRumblePattern[15];
};
} // namespace LUS
