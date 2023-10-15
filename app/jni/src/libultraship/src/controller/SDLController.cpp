#define NOMINMAX

#include "SDLController.h"

#include <spdlog/spdlog.h>
#include <Utils/StringHelper.h>

#ifdef _MSC_VER
#define strdup _strdup
#endif

#define MAX_SDL_RANGE (float)INT16_MAX

// NOLINTNEXTLINE
auto format_as(SDL_GameControllerAxis a) {
    return fmt::underlying(a);
}

namespace LUS {

SDLController::SDLController(int32_t deviceIndex) : Controller(deviceIndex), mController(nullptr) {
}

bool SDLController::Open() {
    const auto newCont = SDL_GameControllerOpen(mDeviceIndex);

    // We failed to load the controller. Go to next.
    if (newCont == nullptr) {
        SPDLOG_ERROR("SDL Controller failed to open: ({})", SDL_GetError());
        return false;
    }

    mSupportsGyro = false;
    if (SDL_GameControllerHasSensor(newCont, SDL_SENSOR_GYRO)) {
        SDL_GameControllerSetSensorEnabled(newCont, SDL_SENSOR_GYRO, SDL_TRUE);
        mSupportsGyro = true;
    }

    char guidBuf[33];
    SDL_JoystickGetGUIDString(SDL_JoystickGetDeviceGUID(mDeviceIndex), guidBuf, sizeof(guidBuf));
    mController = newCont;

#ifdef __SWITCH__
    mGuid = StringHelper::Sprintf("%s:%d", guidBuf, mDeviceIndex);
    mControllerName = StringHelper::Sprintf("%s #%d", SDL_GameControllerNameForIndex(mDeviceIndex), mDeviceIndex + 1);
#else
    mGuid = std::string(guidBuf);
    mControllerName = std::string(SDL_GameControllerNameForIndex(mDeviceIndex));
#endif
    return true;
}

bool SDLController::Close() {
    if (mController != nullptr && SDL_WasInit(SDL_INIT_GAMECONTROLLER)) {
        if (CanRumble()) {
            SDL_GameControllerRumble(mController, 0, 0, 0);
        }
        SDL_GameControllerClose(mController);
    }
    mController = nullptr;

    return true;
}

float SDLController::NormaliseStickValue(float axisValue) {
    // scale {-32768 ... +32767} to {-MAX_AXIS_RANGE ... +MAX_AXIS_RANGE}
    return axisValue * MAX_AXIS_RANGE / MAX_SDL_RANGE;
}

void SDLController::NormalizeStickAxis(SDL_GameControllerAxis axisX, SDL_GameControllerAxis axisY, int32_t portIndex) {
    const auto axisValueX = SDL_GameControllerGetAxis(mController, axisX);
    const auto axisValueY = SDL_GameControllerGetAxis(mController, axisY);

    auto ax = NormaliseStickValue(axisValueX);
    auto ay = NormaliseStickValue(axisValueY);

    if (axisX == SDL_CONTROLLER_AXIS_LEFTX) {
        GetLeftStickX(portIndex) = +ax;
        GetLeftStickY(portIndex) = -ay;
    } else if (axisX == SDL_CONTROLLER_AXIS_RIGHTX) {
        GetRightStickX(portIndex) = +ax;
        GetRightStickY(portIndex) = -ay;
    }
}

int32_t SDLController::ReadRawPress() {
    SDL_GameControllerUpdate();

    for (int32_t i = SDL_CONTROLLER_BUTTON_A; i < SDL_CONTROLLER_BUTTON_MAX; i++) {
        if (SDL_GameControllerGetButton(mController, static_cast<SDL_GameControllerButton>(i))) {
            return i;
        }
    }

    for (int32_t i = SDL_CONTROLLER_AXIS_LEFTX; i < SDL_CONTROLLER_AXIS_MAX; i++) {
        const auto axis = static_cast<SDL_GameControllerAxis>(i);
        const auto axisValue = SDL_GameControllerGetAxis(mController, axis) / 32767.0f;

        if (axisValue < -0.7f) {
            return -(axis + AXIS_SCANCODE_BIT);
        }

        if (axisValue > 0.7f) {
            return (axis + AXIS_SCANCODE_BIT);
        }
    }

    return -1;
}

void SDLController::ReadDevice(int32_t portIndex) {
    auto profile = GetProfile(portIndex);

    SDL_GameControllerUpdate();

    // If the controller is disconnected, close it.
    if (mController != nullptr && !SDL_GameControllerGetAttached(mController)) {
        Close();
    }

    // Attempt to load the controller if it's not loaded
    if (mController == nullptr) {
        // If we failed to load the controller, don't process it.
        if (!Open()) {
            return;
        }
    }

    if (mSupportsGyro && profile->UseGyro) {

        float gyroData[3];
        SDL_GameControllerGetSensorData(mController, SDL_SENSOR_GYRO, gyroData, 3);

        float gyroDriftX = profile->GyroData[DRIFT_X] / 100.0f;
        float gyroDriftY = profile->GyroData[DRIFT_Y] / 100.0f;
        const float gyroSensitivity = profile->GyroData[GYRO_SENSITIVITY];

        if (gyroDriftX == 0) {
            gyroDriftX = gyroData[0];
        }

        if (gyroDriftY == 0) {
            gyroDriftY = gyroData[1];
        }

        profile->GyroData[DRIFT_X] = gyroDriftX * 100.0f;
        profile->GyroData[DRIFT_Y] = gyroDriftY * 100.0f;

        GetGyroX(portIndex) = gyroData[0] - gyroDriftX;
        GetGyroY(portIndex) = gyroData[1] - gyroDriftY;

        GetGyroX(portIndex) *= gyroSensitivity;
        GetGyroY(portIndex) *= gyroSensitivity;
    } else {
        GetGyroX(portIndex) = 0;
        GetGyroY(portIndex) = 0;
    }

    GetPressedButtons(portIndex) = 0;

    for (int32_t i = SDL_CONTROLLER_BUTTON_A; i < SDL_CONTROLLER_BUTTON_MAX; i++) {
        if (profile->Mappings.contains(i)) {
            if (SDL_GameControllerGetButton(mController, static_cast<SDL_GameControllerButton>(i))) {
                GetPressedButtons(portIndex) |= profile->Mappings[i];
            } else {
                GetPressedButtons(portIndex) &= ~profile->Mappings[i];
            }
        }
    }

    SDL_GameControllerAxis leftStickAxisX = SDL_CONTROLLER_AXIS_INVALID;
    SDL_GameControllerAxis leftStickAxisY = SDL_CONTROLLER_AXIS_INVALID;

    SDL_GameControllerAxis rightStickAxisX = SDL_CONTROLLER_AXIS_INVALID;
    SDL_GameControllerAxis rightStickAxisY = SDL_CONTROLLER_AXIS_INVALID;

    for (int32_t i = SDL_CONTROLLER_AXIS_LEFTX; i < SDL_CONTROLLER_AXIS_MAX; i++) {
        const auto axis = static_cast<SDL_GameControllerAxis>(i);
        const auto posScancode = i | AXIS_SCANCODE_BIT;
        const auto negScancode = -posScancode;
        const auto axisMinimumPress = profile->AxisMinimumPress[i];
        const auto axisDeadzone = profile->AxisDeadzones[i];
        const auto posButton = profile->Mappings[posScancode];
        const auto negButton = profile->Mappings[negScancode];
        const auto axisValue = SDL_GameControllerGetAxis(mController, axis);

#ifdef TARGET_WEB
        // Firefox has a bug: https://bugzilla.mozilla.org/show_bug.cgi?id=1606562
        // It sets down y to 32768.0f / 32767.0f, which is greater than the allowed 1.0f,
        // which SDL then converts to a int16_t by multiplying by 32767.0f, which overflows into -32768.
        // Maximum up will hence never become -32768 with the current version of SDL2,
        // so this workaround should be safe in compliant browsers.
        if (AxisValue == -32768) {
            AxisValue = 32767;
        }
#endif

        if (!(posButton == BTN_STICKLEFT || posButton == BTN_STICKRIGHT || posButton == BTN_STICKUP ||
              posButton == BTN_STICKDOWN || negButton == BTN_STICKLEFT || negButton == BTN_STICKRIGHT ||
              negButton == BTN_STICKUP || negButton == BTN_STICKDOWN || posButton == BTN_VSTICKLEFT ||
              posButton == BTN_VSTICKRIGHT || posButton == BTN_VSTICKUP || posButton == BTN_VSTICKDOWN ||
              negButton == BTN_VSTICKLEFT || negButton == BTN_VSTICKRIGHT || negButton == BTN_VSTICKUP ||
              negButton == BTN_VSTICKDOWN)) {

            // The axis is being treated as a "button"

            auto axisComparableValue = axisValue;
            auto axisMinValue = axisMinimumPress;
            if (profile->UseStickDeadzoneForButtons) {
                axisComparableValue = NormaliseStickValue(axisValue);
                axisMinValue = axisDeadzone;
            }

            if (axisComparableValue > axisMinValue) {
                GetPressedButtons(portIndex) |= posButton;
                GetPressedButtons(portIndex) &= ~negButton;
            } else if (axisComparableValue < -axisMinValue) {
                GetPressedButtons(portIndex) &= ~posButton;
                GetPressedButtons(portIndex) |= negButton;
            } else {
                GetPressedButtons(portIndex) &= ~posButton;
                GetPressedButtons(portIndex) &= ~negButton;
            }
        } else {
            // The axis is being treated as a "stick"

            // Left stick
            if (posButton == BTN_STICKLEFT || posButton == BTN_STICKRIGHT) {
                if (leftStickAxisX != SDL_CONTROLLER_AXIS_INVALID && leftStickAxisX != axis) {
                    SPDLOG_TRACE("Invalid PosStickX configured. Neg was {} and Pos is {}", leftStickAxisX, axis);
                }

                leftStickAxisX = axis;
            }

            if (posButton == BTN_STICKUP || posButton == BTN_STICKDOWN) {
                if (leftStickAxisY != SDL_CONTROLLER_AXIS_INVALID && leftStickAxisY != axis) {
                    SPDLOG_TRACE("Invalid PosStickY configured. Neg was {} and Pos is {}", leftStickAxisY, axis);
                }

                leftStickAxisY = axis;
            }

            if (negButton == BTN_STICKLEFT || negButton == BTN_STICKRIGHT) {
                if (leftStickAxisX != SDL_CONTROLLER_AXIS_INVALID && leftStickAxisX != axis) {
                    SPDLOG_TRACE("Invalid NegStickX configured. Pos was {} and Neg is {}", leftStickAxisX, axis);
                }

                leftStickAxisX = axis;
            }

            if (negButton == BTN_STICKUP || negButton == BTN_STICKDOWN) {
                if (leftStickAxisY != SDL_CONTROLLER_AXIS_INVALID && leftStickAxisY != axis) {
                    SPDLOG_TRACE("Invalid NegStickY configured. Pos was {} and Neg is {}", leftStickAxisY, axis);
                }

                leftStickAxisY = axis;
            }

            // Right Stick
            if (posButton == BTN_VSTICKLEFT || posButton == BTN_VSTICKRIGHT) {
                if (rightStickAxisX != SDL_CONTROLLER_AXIS_INVALID && rightStickAxisX != axis) {
                    SPDLOG_TRACE("Invalid PosStickX configured. Neg was {} and Pos is {}", rightStickAxisX, axis);
                }

                rightStickAxisX = axis;
            }

            if (posButton == BTN_VSTICKUP || posButton == BTN_VSTICKDOWN) {
                if (rightStickAxisY != SDL_CONTROLLER_AXIS_INVALID && rightStickAxisY != axis) {
                    SPDLOG_TRACE("Invalid PosStickY configured. Neg was {} and Pos is {}", rightStickAxisY, axis);
                }

                rightStickAxisY = axis;
            }

            if (negButton == BTN_VSTICKLEFT || negButton == BTN_VSTICKRIGHT) {
                if (rightStickAxisX != SDL_CONTROLLER_AXIS_INVALID && rightStickAxisX != axis) {
                    SPDLOG_TRACE("Invalid NegStickX configured. Pos was {} and Neg is {}", rightStickAxisX, axis);
                }

                rightStickAxisX = axis;
            }

            if (negButton == BTN_VSTICKUP || negButton == BTN_VSTICKDOWN) {
                if (rightStickAxisY != SDL_CONTROLLER_AXIS_INVALID && rightStickAxisY != axis) {
                    SPDLOG_TRACE("Invalid NegStickY configured. Pos was {} and Neg is {}", rightStickAxisY, axis);
                }

                rightStickAxisY = axis;
            }
        }
    }

    if (leftStickAxisX != SDL_CONTROLLER_AXIS_INVALID && leftStickAxisY != SDL_CONTROLLER_AXIS_INVALID) {
        NormalizeStickAxis(leftStickAxisX, leftStickAxisY, portIndex);
    }

    if (rightStickAxisX != SDL_CONTROLLER_AXIS_INVALID && rightStickAxisY != SDL_CONTROLLER_AXIS_INVALID) {
        NormalizeStickAxis(rightStickAxisX, rightStickAxisY, portIndex);
    }
}

int32_t SDLController::SetRumble(int32_t portIndex, bool rumble) {
    if (!CanRumble()) {
        return -1000;
    }

    if (!GetProfile(portIndex)->UseRumble) {
        return -1001;
    }

    mIsRumbling = rumble;
    if (mIsRumbling) {
        float rumbleStrength = GetProfile(portIndex)->RumbleStrength;
        return SDL_GameControllerRumble(mController, 0xFFFF * rumbleStrength, 0xFFFF * rumbleStrength, 0);
    } else {
        return SDL_GameControllerRumble(mController, 0, 0, 0);
    }
}

int32_t SDLController::SetLedColor(int32_t portIndex, Color_RGB8 color) {
    if (!CanSetLed()) {
        return -1000;
    }

    mLedColor = color;
    return SDL_JoystickSetLED(SDL_GameControllerGetJoystick(mController), mLedColor.r, mLedColor.g, mLedColor.b);
}

const std::string SDLController::GetButtonName(int32_t portIndex, int32_t n64bitmask) {
    char buffer[50];
    // OTRTODO: This should get the scancode of all bits in the mask.
    std::map<int32_t, int32_t>& mappings = GetProfile(portIndex)->Mappings;

    const auto find =
        std::find_if(mappings.begin(), mappings.end(),
                     [n64bitmask](const std::pair<int32_t, int32_t>& pair) { return pair.second == n64bitmask; });

    if (find == mappings.end()) {
        return "Unknown";
    }

    int btn = abs(find->first);

    if (btn >= AXIS_SCANCODE_BIT) {
        btn -= AXIS_SCANCODE_BIT;

        snprintf(buffer, sizeof(buffer), "%s%s", sAxisNames[btn], find->first > 0 ? "+" : "-");
        return buffer;
    }

    snprintf(buffer, sizeof(buffer), "Button %d", btn);
    return buffer;
}

void SDLController::CreateDefaultBinding(int32_t portIndex) {
    auto profile = GetProfile(portIndex);
    profile->Mappings.clear();
    profile->AxisDeadzones.clear();
    profile->AxisMinimumPress.clear();
    profile->GyroData.clear();

    profile->Version = DEVICE_PROFILE_CURRENT_VERSION;
    profile->UseRumble = true;
    profile->RumbleStrength = 1.0f;
    profile->UseGyro = false;

    profile->Mappings[SDL_CONTROLLER_AXIS_RIGHTX | AXIS_SCANCODE_BIT] = BTN_CRIGHT;
    profile->Mappings[-(SDL_CONTROLLER_AXIS_RIGHTX | AXIS_SCANCODE_BIT)] = BTN_CLEFT;
    profile->Mappings[SDL_CONTROLLER_AXIS_RIGHTY | AXIS_SCANCODE_BIT] = BTN_CDOWN;
    profile->Mappings[-(SDL_CONTROLLER_AXIS_RIGHTY | AXIS_SCANCODE_BIT)] = BTN_CUP;
    profile->Mappings[SDL_CONTROLLER_AXIS_LEFTX | AXIS_SCANCODE_BIT] = BTN_STICKRIGHT;
    profile->Mappings[-(SDL_CONTROLLER_AXIS_LEFTX | AXIS_SCANCODE_BIT)] = BTN_STICKLEFT;
    profile->Mappings[SDL_CONTROLLER_AXIS_LEFTY | AXIS_SCANCODE_BIT] = BTN_STICKDOWN;
    profile->Mappings[-(SDL_CONTROLLER_AXIS_LEFTY | AXIS_SCANCODE_BIT)] = BTN_STICKUP;
    profile->Mappings[SDL_CONTROLLER_AXIS_TRIGGERRIGHT | AXIS_SCANCODE_BIT] = BTN_R;
    profile->Mappings[SDL_CONTROLLER_AXIS_TRIGGERLEFT | AXIS_SCANCODE_BIT] = BTN_Z;
    profile->Mappings[SDL_CONTROLLER_BUTTON_LEFTSHOULDER] = BTN_L;
    profile->Mappings[SDL_CONTROLLER_BUTTON_DPAD_RIGHT] = BTN_DRIGHT;
    profile->Mappings[SDL_CONTROLLER_BUTTON_DPAD_LEFT] = BTN_DLEFT;
    profile->Mappings[SDL_CONTROLLER_BUTTON_DPAD_DOWN] = BTN_DDOWN;
    profile->Mappings[SDL_CONTROLLER_BUTTON_DPAD_UP] = BTN_DUP;
    profile->Mappings[SDL_CONTROLLER_BUTTON_START] = BTN_START;
    profile->Mappings[SDL_CONTROLLER_BUTTON_B] = BTN_B;
    profile->Mappings[SDL_CONTROLLER_BUTTON_A] = BTN_A;
    profile->Mappings[SDL_CONTROLLER_BUTTON_LEFTSTICK] = BTN_MODIFIER1;
    profile->Mappings[SDL_CONTROLLER_BUTTON_RIGHTSTICK] = BTN_MODIFIER2;

    for (int32_t i = SDL_CONTROLLER_AXIS_LEFTX; i < SDL_CONTROLLER_AXIS_MAX; i++) {
        profile->AxisDeadzones[i] = 16.0f;
        profile->AxisMinimumPress[i] = 7680.0f;
    }

    profile->GyroData[DRIFT_X] = 0.0f;
    profile->GyroData[DRIFT_Y] = 0.0f;
    profile->GyroData[GYRO_SENSITIVITY] = 1.0f;
}

bool SDLController::Connected() const {
    return mController != nullptr;
}

bool SDLController::CanGyro() const {
    return mSupportsGyro;
}

bool SDLController::CanRumble() const {
#if SDL_COMPILEDVERSION >= SDL_VERSIONNUM(2, 0, 18)
    return SDL_GameControllerHasRumble(mController);
#endif
    return false;
}

bool SDLController::CanSetLed() const {
    return SDL_GameControllerHasLED(mController);
}

void SDLController::ClearRawPress() {
}
} // namespace LUS
