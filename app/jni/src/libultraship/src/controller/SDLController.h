#pragma once
#include "Controller.h"
#include <SDL2/SDL.h>

namespace LUS {
class SDLController : public Controller {
  public:
    SDLController(int32_t deviceIndex);
    void ReadDevice(int32_t portIndex) override;
    const std::string GetButtonName(int32_t portIndex, int32_t n64bitmask) override;
    int32_t SetRumble(int32_t portIndex, bool rumble) override;
    int32_t SetLedColor(int32_t portIndex, Color_RGB8 color) override;
    bool Connected() const override;
    bool CanGyro() const override;
    bool CanRumble() const override;
    bool CanSetLed() const override;
    bool Open();
    void ClearRawPress() override;
    int32_t ReadRawPress() override;

  protected:
    inline static const char* sAxisNames[] = { "Left Stick X", "Left Stick Y",  "Right Stick X", "Right Stick Y",
                                               "Left Trigger", "Right Trigger", "Start Button" };

    void CreateDefaultBinding(int32_t portIndex) override;

  private:
    SDL_GameController* mController;
    bool mSupportsGyro;
    float NormaliseStickValue(float axisValue);
    void NormalizeStickAxis(SDL_GameControllerAxis axisX, SDL_GameControllerAxis axisY, int32_t portIndex);
    bool Close();
};
} // namespace LUS
