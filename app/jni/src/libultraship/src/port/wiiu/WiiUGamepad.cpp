#ifdef __WIIU__
#include "WiiUGamepad.h"
#include <cstring>
#include <algorithm>
#include "WiiUImpl.h"

namespace LUS {
WiiUGamepad::WiiUGamepad(int32_t deviceIndex)
    : Controller(deviceIndex), mConnected(true), mRumblePatternStrength(1.0f) {
    memset(mRumblePattern, 0xff, sizeof(mRumblePattern));
    mGuid = "WiiUGamepad";
    mControllerName = "Wii U GamePad";
}

bool WiiUGamepad::Open() {
    VPADReadError error;
    VPADStatus* status = LUS::WiiU::GetVPADStatus(&error);
    if (!status || error == VPAD_READ_INVALID_CONTROLLER) {
        Close();
        return false;
    }

    mConnected = true;
    mControllerName = "Wii U GamePad";

    return true;
}

void WiiUGamepad::Close() {
    mConnected = false;
    mControllerName = "Wii U Gamepad (Disconnected)";

    for (int i = 0; i < MAXCONTROLLERS; i++) {
        GetPressedButtons(i) = 0;
        GetLeftStickX(i) = 0;
        GetLeftStickY(i) = 0;
        GetRightStickX(i) = 0;
        GetRightStickY(i) = 0;
        GetGyroX(i) = 0;
        GetGyroY(i) = 0;
    }
}

void WiiUGamepad::ReadDevice(int32_t portIndex) {
    auto profile = GetProfile(portIndex);

    VPADReadError error;
    VPADStatus* status = LUS::WiiU::GetVPADStatus(&error);
    if (!status) {
        Close();
        return;
    }

    GetPressedButtons(portIndex) = 0;
    GetLeftStickX(portIndex) = 0;
    GetLeftStickY(portIndex) = 0;
    GetRightStickX(portIndex) = 0;
    GetRightStickY(portIndex) = 0;
    GetGyroX(portIndex) = 0;
    GetGyroY(portIndex) = 0;

    if (error != VPAD_READ_SUCCESS) {
        return;
    }

    int16_t stickX = 0;
    int16_t stickY = 0;
    int16_t camX = 0;
    int16_t camY = 0;

    for (uint32_t i = VPAD_BUTTON_SYNC; i <= VPAD_STICK_L_EMULATION_LEFT; i <<= 1) {
        if (profile->Mappings.contains(i)) {
            // check if the stick is mapped to an analog stick
            if (i >= VPAD_STICK_R_EMULATION_DOWN) {
                float axisX = i >= VPAD_STICK_L_EMULATION_DOWN ? status->leftStick.x : status->rightStick.x;
                float axisY = i >= VPAD_STICK_L_EMULATION_DOWN ? status->leftStick.y : status->rightStick.y;

                if (profile->Mappings[i] == BTN_STICKRIGHT || profile->Mappings[i] == BTN_STICKLEFT) {
                    stickX = axisX * MAX_AXIS_RANGE;
                    continue;
                } else if (profile->Mappings[i] == BTN_STICKDOWN || profile->Mappings[i] == BTN_STICKUP) {
                    stickY = axisY * MAX_AXIS_RANGE;
                    continue;
                } else if (profile->Mappings[i] == BTN_VSTICKRIGHT || profile->Mappings[i] == BTN_VSTICKLEFT) {
                    camX = axisX * MAX_AXIS_RANGE;
                    continue;
                } else if (profile->Mappings[i] == BTN_VSTICKDOWN || profile->Mappings[i] == BTN_VSTICKUP) {
                    camY = axisY * MAX_AXIS_RANGE;
                    continue;
                }
            }

            if (status->hold & i) {
                GetPressedButtons(portIndex) |= profile->Mappings[i];
            }
        }
    }

    if (stickX || stickY) {
        GetLeftStickX(portIndex) = stickX;
        GetLeftStickY(portIndex) = stickY;
    }

    if (camX || camY) {
        GetRightStickX(portIndex) = camX;
        GetRightStickY(portIndex) = camY;
    }

    if (profile->UseGyro) {
        float gyroX = status->gyro.x * -8.0f;
        float gyroY = status->gyro.z * 8.0f;

        float gyro_drift_x = profile->GyroData[DRIFT_X] / 100.0f;
        float gyro_drift_y = profile->GyroData[DRIFT_Y] / 100.0f;
        const float gyro_sensitivity = profile->GyroData[GYRO_SENSITIVITY];

        if (gyro_drift_x == 0) {
            gyro_drift_x = gyroX;
        }

        if (gyro_drift_y == 0) {
            gyro_drift_y = gyroY;
        }

        profile->GyroData[DRIFT_X] = gyro_drift_x * 100.0f;
        profile->GyroData[DRIFT_Y] = gyro_drift_y * 100.0f;

        GetGyroX(portIndex) = gyroX - gyro_drift_x;
        GetGyroY(portIndex) = gyroY - gyro_drift_y;

        GetGyroX(portIndex) *= gyro_sensitivity;
        GetGyroY(portIndex) *= gyro_sensitivity;
    }
}

int32_t WiiUGamepad::SetRumble(int32_t portIndex, bool rumble) {
    auto profile = GetProfile(portIndex);

    if (!CanRumble()) {
        return -1000;
    }

    if (!profile->UseRumble) {
        return -1001;
    }

    int32_t patternSize = sizeof(mRumblePattern) * 8;

    // update rumble pattern if strength changed
    if (mRumblePatternStrength != profile->RumbleStrength) {
        mRumblePatternStrength = profile->RumbleStrength;
        if (mRumblePatternStrength > 1.0f) {
            mRumblePatternStrength = 1.0f;
        } else if (mRumblePatternStrength < 0.0f) {
            mRumblePatternStrength = 0.0f;
        }

        memset(mRumblePattern, 0, sizeof(mRumblePattern));

        // distribute wanted amount of bits equally in pattern
        float scale = (mRumblePatternStrength * (1.0f - 0.3f)) + 0.3f;
        int32_t bitcnt = patternSize * scale;
        for (int32_t i = 0; i < bitcnt; i++) {
            int32_t bitpos = ((i * patternSize) / bitcnt) % patternSize;
            mRumblePattern[bitpos / 8] |= 1 << (bitpos % 8);
        }
    }

    mIsRumbling = rumble;
    return VPADControlMotor(VPAD_CHAN_0, mRumblePattern, mIsRumbling ? patternSize : 0);
}

int32_t WiiUGamepad::SetLedColor(int32_t portIndex, Color_RGB8 color) {
    return -1000;
}

void WiiUGamepad::ClearRawPress() {
    // Clear already triggered buttons
    VPADReadError error;
    VPADStatus* status = LUS::WiiU::GetVPADStatus(&error);
    if (status) {
        status->trigger = 0;
    }
}

int32_t WiiUGamepad::ReadRawPress() {
    VPADReadError error;
    VPADStatus* status = LUS::WiiU::GetVPADStatus(&error);
    if (!status || error != VPAD_READ_SUCCESS) {
        return -1;
    }

    for (uint32_t i = VPAD_BUTTON_SYNC; i <= VPAD_BUTTON_STICK_L; i <<= 1) {
        if (status->trigger & i) {
            return i;
        }
    }

    if (status->leftStick.x > 0.7f) {
        return VPAD_STICK_L_EMULATION_RIGHT;
    }
    if (status->leftStick.x < -0.7f) {
        return VPAD_STICK_L_EMULATION_LEFT;
    }
    if (status->leftStick.y > 0.7f) {
        return VPAD_STICK_L_EMULATION_UP;
    }
    if (status->leftStick.y < -0.7f) {
        return VPAD_STICK_L_EMULATION_DOWN;
    }

    if (status->rightStick.x > 0.7f) {
        return VPAD_STICK_R_EMULATION_RIGHT;
    }
    if (status->rightStick.x < -0.7f) {
        return VPAD_STICK_R_EMULATION_LEFT;
    }
    if (status->rightStick.y > 0.7f) {
        return VPAD_STICK_R_EMULATION_UP;
    }
    if (status->rightStick.y < -0.7f) {
        return VPAD_STICK_R_EMULATION_DOWN;
    }

    return -1;
}

const std::string WiiUGamepad::GetButtonName(int32_t portIndex, int32_t n64bitmask) {
    std::map<int32_t, int32_t>& mappings = GetProfile(portIndex)->Mappings;
    const auto find =
        std::find_if(mappings.begin(), mappings.end(),
                     [n64bitmask](const std::pair<int32_t, int32_t>& pair) { return pair.second == n64bitmask; });

    if (find == mappings.end())
        return "Unknown";

    uint32_t btn = find->first;
    switch (btn) {
        case VPAD_BUTTON_A:
            return "A";
        case VPAD_BUTTON_B:
            return "B";
        case VPAD_BUTTON_X:
            return "X";
        case VPAD_BUTTON_Y:
            return "Y";
        case VPAD_BUTTON_LEFT:
            return "D-pad Left";
        case VPAD_BUTTON_RIGHT:
            return "D-pad Right";
        case VPAD_BUTTON_UP:
            return "D-pad Up";
        case VPAD_BUTTON_DOWN:
            return "D-pad Down";
        case VPAD_BUTTON_ZL:
            return "ZL";
        case VPAD_BUTTON_ZR:
            return "ZR";
        case VPAD_BUTTON_L:
            return "L";
        case VPAD_BUTTON_R:
            return "R";
        case VPAD_BUTTON_PLUS:
            return "+ (START)";
        case VPAD_BUTTON_MINUS:
            return "- (SELECT)";
        case VPAD_BUTTON_STICK_R:
            return "Stick Button R";
        case VPAD_BUTTON_STICK_L:
            return "Stick Button L";
        case VPAD_STICK_R_EMULATION_LEFT:
            return "Right Stick Left";
        case VPAD_STICK_R_EMULATION_RIGHT:
            return "Right Stick Right";
        case VPAD_STICK_R_EMULATION_UP:
            return "Right Stick Up";
        case VPAD_STICK_R_EMULATION_DOWN:
            return "Right Stick Down";
        case VPAD_STICK_L_EMULATION_LEFT:
            return "Left Stick Left";
        case VPAD_STICK_L_EMULATION_RIGHT:
            return "Left Stick Right";
        case VPAD_STICK_L_EMULATION_UP:
            return "Left Stick Up";
        case VPAD_STICK_L_EMULATION_DOWN:
            return "Left Stick Down";
    }

    return "Unknown";
}

void WiiUGamepad::CreateDefaultBinding(int32_t portIndex) {
    auto profile = GetProfile(portIndex);
    profile->Mappings.clear();

    profile->Version = DEVICE_PROFILE_CURRENT_VERSION;
    profile->UseRumble = true;
    profile->RumbleStrength = 1.0f;
    profile->UseGyro = false;

    profile->Mappings[VPAD_STICK_R_EMULATION_RIGHT] = BTN_CRIGHT;
    profile->Mappings[VPAD_STICK_R_EMULATION_LEFT] = BTN_CLEFT;
    profile->Mappings[VPAD_STICK_R_EMULATION_DOWN] = BTN_CDOWN;
    profile->Mappings[VPAD_STICK_R_EMULATION_UP] = BTN_CUP;
    profile->Mappings[VPAD_BUTTON_ZR] = BTN_R;
    profile->Mappings[VPAD_BUTTON_L] = BTN_L;
    profile->Mappings[VPAD_BUTTON_RIGHT] = BTN_DRIGHT;
    profile->Mappings[VPAD_BUTTON_LEFT] = BTN_DLEFT;
    profile->Mappings[VPAD_BUTTON_DOWN] = BTN_DDOWN;
    profile->Mappings[VPAD_BUTTON_UP] = BTN_DUP;
    profile->Mappings[VPAD_BUTTON_PLUS] = BTN_START;
    profile->Mappings[VPAD_BUTTON_ZL] = BTN_Z;
    profile->Mappings[VPAD_BUTTON_B] = BTN_B;
    profile->Mappings[VPAD_BUTTON_A] = BTN_A;
    profile->Mappings[VPAD_STICK_L_EMULATION_RIGHT] = BTN_STICKRIGHT;
    profile->Mappings[VPAD_STICK_L_EMULATION_LEFT] = BTN_STICKLEFT;
    profile->Mappings[VPAD_STICK_L_EMULATION_DOWN] = BTN_STICKDOWN;
    profile->Mappings[VPAD_STICK_L_EMULATION_UP] = BTN_STICKUP;

    for (int i = 0; i < 4; i++) {
        profile->AxisDeadzones[i] = 0.0f;
        profile->AxisMinimumPress[i] = 7680.0f;
    }

    profile->GyroData[GYRO_SENSITIVITY] = 1.0f;
    profile->GyroData[DRIFT_X] = 0.0f;
    profile->GyroData[DRIFT_Y] = 0.0f;
}

bool WiiUGamepad::Connected() const {
    return mConnected;
};

bool WiiUGamepad::CanGyro() const {
    return true;
}

bool WiiUGamepad::CanRumble() const {
    return true;
};

bool WiiUGamepad::CanSetLed() const {
    return false;
}
} // namespace LUS
#endif
