#ifdef __WIIU__
#include "WiiUController.h"
#include "window/Window.h"
#include <algorithm>
#include "WiiUImpl.h"

namespace LUS {
WiiUController::WiiUController(int32_t deviceIndex, WPADChan chan) : Controller(deviceIndex), mChan(chan) {
    mConnected = false;
    mExtensionType = (WPADExtensionType)-1;
    mControllerName = std::string("Wii U Controller ") + std::to_string((int)mChan) + " (Disconnected)";
}

bool WiiUController::Open() {
    KPADError error;
    KPADStatus* status = LUS::WiiU::GetKPADStatus(mChan, &error);
    if (!status || error != KPAD_ERROR_OK) {
        Close();
        return false;
    }

    mConnected = true;
    mControllerName = std::string("Wii U Controller ") + std::to_string((int)mChan);
    mExtensionType = (WPADExtensionType)status->extensionType;

    // Create a GUID and name based on extension and channel
    mGuid = std::string("WiiU") + GetControllerExtensionName() + std::to_string((int)mChan);
    mControllerName =
        std::string("Wii U ") + GetControllerExtensionName() + std::string(" ") + std::to_string((int)mChan);

    return true;
}

void WiiUController::Close() {
    mConnected = false;
    mControllerName = std::string("Wii U Controller ") + std::to_string((int)mChan) + " (Disconnected)";
    mExtensionType = (WPADExtensionType)-1;

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

void WiiUController::ReadDevice(int32_t portIndex) {
    auto profile = GetProfile(portIndex);

    KPADError error;
    KPADStatus* status = LUS::WiiU::GetKPADStatus(mChan, &error);
    if (!status) {
        Close();
        return;
    }

    // Make sure the controller type doesn't change after opening
    if (status->extensionType != mExtensionType) {
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

    if (error != KPAD_ERROR_OK) {
        return;
    }

    int16_t stickX = 0;
    int16_t stickY = 0;
    int16_t camX = 0;
    int16_t camY = 0;

    switch (mExtensionType) {
        case WPAD_EXT_PRO_CONTROLLER:
            for (uint32_t i = WPAD_PRO_BUTTON_UP; i <= WPAD_PRO_STICK_R_EMULATION_UP; i <<= 1) {
                if (profile->Mappings.contains(i)) {
                    // check if the stick is mapped to an analog stick
                    if (i >= WPAD_PRO_STICK_L_EMULATION_LEFT) {
                        float axisX =
                            i >= WPAD_PRO_STICK_R_EMULATION_LEFT ? status->pro.rightStick.x : status->pro.leftStick.x;
                        float axisY =
                            i >= WPAD_PRO_STICK_R_EMULATION_LEFT ? status->pro.rightStick.y : status->pro.leftStick.y;

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

                    if (status->pro.hold & i) {
                        GetPressedButtons(portIndex) |= profile->Mappings[i];
                    }
                }
            }
            break;
        case WPAD_EXT_CLASSIC:
        case WPAD_EXT_MPLUS_CLASSIC:
            for (uint32_t i = WPAD_CLASSIC_BUTTON_UP; i <= WPAD_CLASSIC_STICK_R_EMULATION_UP; i <<= 1) {
                if (profile->Mappings.contains(i)) {
                    // check if the stick is mapped to an analog stick
                    if (i >= WPAD_CLASSIC_STICK_L_EMULATION_LEFT) {
                        float axisX = i >= WPAD_CLASSIC_STICK_R_EMULATION_LEFT ? status->classic.rightStick.x
                                                                               : status->classic.leftStick.x;
                        float axisY = i >= WPAD_CLASSIC_STICK_R_EMULATION_LEFT ? status->classic.rightStick.y
                                                                               : status->classic.leftStick.y;

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

                    if (status->classic.hold & i) {
                        GetPressedButtons(portIndex) |= profile->Mappings[i];
                    }
                }
            }
            break;
        case WPAD_EXT_NUNCHUK:
        case WPAD_EXT_MPLUS_NUNCHUK:
        case WPAD_EXT_MPLUS:
        case WPAD_EXT_CORE:
            for (uint32_t i = WPAD_BUTTON_LEFT; i <= WPAD_BUTTON_HOME; i <<= 1) {
                if (profile->Mappings.contains(i)) {
                    if (status->hold & i) {
                        GetPressedButtons(portIndex) |= profile->Mappings[i];
                    }
                }
            }
            stickX += status->nunchuck.stick.x * MAX_AXIS_RANGE;
            stickY += status->nunchuck.stick.y * MAX_AXIS_RANGE;
            break;
    }

    if (stickX || stickY) {
        GetLeftStickX(portIndex) = stickX;
        GetLeftStickY(portIndex) = stickY;
    }

    if (camX || camY) {
        GetRightStickX(portIndex) = camX;
        GetRightStickY(portIndex) = camY;
    }
}

int32_t WiiUController::SetRumble(int32_t portIndex, bool rumble) {
    if (!CanRumble()) {
        return -1000;
    }

    if (GetProfile(portIndex)->UseRumble) {
        return -1001;
    }

    mIsRumbling = rumble;
    WPADControlMotor(mChan, mIsRumbling);
    return 0;
}

int32_t WiiUController::SetLedColor(int32_t portIndex, Color_RGB8 color) {
    return -1000;
}

void WiiUController::ClearRawPress() {
    // Clear already triggered buttons
    KPADError error;
    KPADStatus* status = LUS::WiiU::GetKPADStatus(mChan, &error);
    if (status) {
        switch (mExtensionType) {
            case WPAD_EXT_PRO_CONTROLLER:
                status->pro.trigger = 0;
                break;
            case WPAD_EXT_CLASSIC:
            case WPAD_EXT_MPLUS_CLASSIC:
                status->classic.trigger = 0;
                break;
            case WPAD_EXT_NUNCHUK:
            case WPAD_EXT_MPLUS_NUNCHUK:
            case WPAD_EXT_MPLUS:
            case WPAD_EXT_CORE:
                status->trigger = 0;
                break;
        }
    }
}

int32_t WiiUController::ReadRawPress() {
    KPADError error;
    KPADStatus* status = LUS::WiiU::GetKPADStatus(mChan, &error);
    if (!status || error != KPAD_ERROR_OK) {
        return -1;
    }

    switch (mExtensionType) {
        case WPAD_EXT_PRO_CONTROLLER:
            for (uint32_t i = WPAD_PRO_BUTTON_UP; i <= WPAD_PRO_STICK_R_EMULATION_UP; i <<= 1) {
                if (status->pro.trigger & i) {
                    return i;
                }
            }

            if (status->pro.leftStick.x > 0.7f) {
                return WPAD_PRO_STICK_L_EMULATION_RIGHT;
            }
            if (status->pro.leftStick.x < -0.7f) {
                return WPAD_PRO_STICK_L_EMULATION_LEFT;
            }
            if (status->pro.leftStick.y > 0.7f) {
                return WPAD_PRO_STICK_L_EMULATION_UP;
            }
            if (status->pro.leftStick.y < -0.7f) {
                return WPAD_PRO_STICK_L_EMULATION_DOWN;
            }

            if (status->pro.rightStick.x > 0.7f) {
                return WPAD_PRO_STICK_R_EMULATION_RIGHT;
            }
            if (status->pro.rightStick.x < -0.7f) {
                return WPAD_PRO_STICK_R_EMULATION_LEFT;
            }
            if (status->pro.rightStick.y > 0.7f) {
                return WPAD_PRO_STICK_R_EMULATION_UP;
            }
            if (status->pro.rightStick.y < -0.7f) {
                return WPAD_PRO_STICK_R_EMULATION_DOWN;
            }
            break;
        case WPAD_EXT_CLASSIC:
        case WPAD_EXT_MPLUS_CLASSIC:
            for (uint32_t i = WPAD_CLASSIC_BUTTON_UP; i <= WPAD_CLASSIC_STICK_R_EMULATION_UP; i <<= 1) {
                if (status->classic.trigger & i) {
                    return i;
                }
            }

            if (status->classic.leftStick.x > 0.7f) {
                return WPAD_CLASSIC_STICK_L_EMULATION_RIGHT;
            }
            if (status->classic.leftStick.x < -0.7f) {
                return WPAD_CLASSIC_STICK_L_EMULATION_LEFT;
            }
            if (status->classic.leftStick.y > 0.7f) {
                return WPAD_CLASSIC_STICK_L_EMULATION_UP;
            }
            if (status->classic.leftStick.y < -0.7f) {
                return WPAD_CLASSIC_STICK_L_EMULATION_DOWN;
            }

            if (status->classic.rightStick.x > 0.7f) {
                return WPAD_CLASSIC_STICK_R_EMULATION_RIGHT;
            }
            if (status->classic.rightStick.x < -0.7f) {
                return WPAD_CLASSIC_STICK_R_EMULATION_LEFT;
            }
            if (status->classic.rightStick.y > 0.7f) {
                return WPAD_CLASSIC_STICK_R_EMULATION_UP;
            }
            if (status->classic.rightStick.y < -0.7f) {
                return WPAD_CLASSIC_STICK_R_EMULATION_DOWN;
            }
            break;
        case WPAD_EXT_NUNCHUK:
        case WPAD_EXT_MPLUS_NUNCHUK:
        case WPAD_EXT_MPLUS:
        case WPAD_EXT_CORE:
            for (uint32_t i = WPAD_BUTTON_LEFT; i <= WPAD_BUTTON_HOME; i <<= 1) {
                if (status->trigger & i) {
                    return i;
                }
            }
            break;
    }

    return -1;
}

const std::string WiiUController::GetButtonName(int32_t portIndex, int32_t n64bitmask) {
    std::map<int32_t, int32_t>& Mappings = GetProfile(portIndex)->Mappings;
    const auto find =
        std::find_if(Mappings.begin(), Mappings.end(),
                     [n64bitmask](const std::pair<int32_t, int32_t>& pair) { return pair.second == n64bitmask; });

    if (find == Mappings.end())
        return "Unknown";

    uint32_t btn = find->first;

    switch (mExtensionType) {
        case WPAD_EXT_PRO_CONTROLLER:
            switch (btn) {
                case WPAD_PRO_BUTTON_A:
                    return "A";
                case WPAD_PRO_BUTTON_B:
                    return "B";
                case WPAD_PRO_BUTTON_X:
                    return "X";
                case WPAD_PRO_BUTTON_Y:
                    return "Y";
                case WPAD_PRO_BUTTON_LEFT:
                    return "D-pad Left";
                case WPAD_PRO_BUTTON_RIGHT:
                    return "D-pad Right";
                case WPAD_PRO_BUTTON_UP:
                    return "D-pad Up";
                case WPAD_PRO_BUTTON_DOWN:
                    return "D-pad Down";
                case WPAD_PRO_TRIGGER_ZL:
                    return "ZL";
                case WPAD_PRO_TRIGGER_ZR:
                    return "ZR";
                case WPAD_PRO_TRIGGER_L:
                    return "L";
                case WPAD_PRO_TRIGGER_R:
                    return "R";
                case WPAD_PRO_BUTTON_PLUS:
                    return "+ (START)";
                case WPAD_PRO_BUTTON_MINUS:
                    return "- (SELECT)";
                case WPAD_PRO_BUTTON_STICK_R:
                    return "Stick Button R";
                case WPAD_PRO_BUTTON_STICK_L:
                    return "Stick Button L";
                case WPAD_PRO_STICK_R_EMULATION_LEFT:
                    return "Right Stick Left";
                case WPAD_PRO_STICK_R_EMULATION_RIGHT:
                    return "Right Stick Right";
                case WPAD_PRO_STICK_R_EMULATION_UP:
                    return "Right Stick Up";
                case WPAD_PRO_STICK_R_EMULATION_DOWN:
                    return "Right Stick Down";
                case WPAD_PRO_STICK_L_EMULATION_LEFT:
                    return "Left Stick Left";
                case WPAD_PRO_STICK_L_EMULATION_RIGHT:
                    return "Left Stick Right";
                case WPAD_PRO_STICK_L_EMULATION_UP:
                    return "Left Stick Up";
                case WPAD_PRO_STICK_L_EMULATION_DOWN:
                    return "Left Stick Down";
            }
            break;
        case WPAD_EXT_CLASSIC:
        case WPAD_EXT_MPLUS_CLASSIC:
            switch (btn) {
                case WPAD_CLASSIC_BUTTON_A:
                    return "A";
                case WPAD_CLASSIC_BUTTON_B:
                    return "B";
                case WPAD_CLASSIC_BUTTON_X:
                    return "X";
                case WPAD_CLASSIC_BUTTON_Y:
                    return "Y";
                case WPAD_CLASSIC_BUTTON_LEFT:
                    return "D-pad Left";
                case WPAD_CLASSIC_BUTTON_RIGHT:
                    return "D-pad Right";
                case WPAD_CLASSIC_BUTTON_UP:
                    return "D-pad Up";
                case WPAD_CLASSIC_BUTTON_DOWN:
                    return "D-pad Down";
                case WPAD_CLASSIC_BUTTON_ZL:
                    return "ZL";
                case WPAD_CLASSIC_BUTTON_ZR:
                    return "ZR";
                case WPAD_CLASSIC_BUTTON_L:
                    return "L";
                case WPAD_CLASSIC_BUTTON_R:
                    return "R";
                case WPAD_CLASSIC_BUTTON_PLUS:
                    return "+ (START)";
                case WPAD_CLASSIC_BUTTON_MINUS:
                    return "- (SELECT)";
                case WPAD_CLASSIC_STICK_R_EMULATION_LEFT:
                    return "Right Stick Left";
                case WPAD_CLASSIC_STICK_R_EMULATION_RIGHT:
                    return "Right Stick Right";
                case WPAD_CLASSIC_STICK_R_EMULATION_UP:
                    return "Right Stick Up";
                case WPAD_CLASSIC_STICK_R_EMULATION_DOWN:
                    return "Right Stick Down";
                case WPAD_CLASSIC_STICK_L_EMULATION_LEFT:
                    return "Left Stick Left";
                case WPAD_CLASSIC_STICK_L_EMULATION_RIGHT:
                    return "Left Stick Right";
                case WPAD_CLASSIC_STICK_L_EMULATION_UP:
                    return "Left Stick Up";
                case WPAD_CLASSIC_STICK_L_EMULATION_DOWN:
                    return "Left Stick Down";
            }
            break;
        case WPAD_EXT_NUNCHUK:
        case WPAD_EXT_MPLUS_NUNCHUK:
        case WPAD_EXT_MPLUS:
        case WPAD_EXT_CORE:
            switch (btn) {
                case WPAD_BUTTON_A:
                    return "A";
                case WPAD_BUTTON_B:
                    return "B";
                case WPAD_BUTTON_1:
                    return "1";
                case WPAD_BUTTON_2:
                    return "2";
                case WPAD_BUTTON_LEFT:
                    return "D-pad Left";
                case WPAD_BUTTON_RIGHT:
                    return "D-pad Right";
                case WPAD_BUTTON_UP:
                    return "D-pad Up";
                case WPAD_BUTTON_DOWN:
                    return "D-pad Down";
                case WPAD_BUTTON_Z:
                    return "Z";
                case WPAD_BUTTON_C:
                    return "C";
                case WPAD_BUTTON_PLUS:
                    return "+ (START)";
                case WPAD_BUTTON_MINUS:
                    return "- (SELECT)";
            }
            break;
    }

    return "Unknown";
}

void WiiUController::CreateDefaultBinding(int32_t portIndex) {
    auto profile = GetProfile(portIndex);
    profile->Mappings.clear();

    profile->UseRumble = true;
    profile->RumbleStrength = 1.0f;
    profile->UseGyro = false;

    switch (mExtensionType) {
        case WPAD_EXT_PRO_CONTROLLER:
            profile->Mappings[WPAD_PRO_STICK_R_EMULATION_RIGHT] = BTN_CRIGHT;
            profile->Mappings[WPAD_PRO_STICK_R_EMULATION_LEFT] = BTN_CLEFT;
            profile->Mappings[WPAD_PRO_STICK_R_EMULATION_DOWN] = BTN_CDOWN;
            profile->Mappings[WPAD_PRO_STICK_R_EMULATION_UP] = BTN_CUP;
            profile->Mappings[WPAD_PRO_TRIGGER_ZR] = BTN_R;
            profile->Mappings[WPAD_PRO_TRIGGER_L] = BTN_L;
            profile->Mappings[WPAD_PRO_BUTTON_RIGHT] = BTN_DRIGHT;
            profile->Mappings[WPAD_PRO_BUTTON_LEFT] = BTN_DLEFT;
            profile->Mappings[WPAD_PRO_BUTTON_DOWN] = BTN_DDOWN;
            profile->Mappings[WPAD_PRO_BUTTON_UP] = BTN_DUP;
            profile->Mappings[WPAD_PRO_BUTTON_PLUS] = BTN_START;
            profile->Mappings[WPAD_PRO_TRIGGER_ZL] = BTN_Z;
            profile->Mappings[WPAD_PRO_BUTTON_B] = BTN_B;
            profile->Mappings[WPAD_PRO_BUTTON_A] = BTN_A;
            profile->Mappings[WPAD_PRO_STICK_L_EMULATION_RIGHT] = BTN_STICKRIGHT;
            profile->Mappings[WPAD_PRO_STICK_L_EMULATION_LEFT] = BTN_STICKLEFT;
            profile->Mappings[WPAD_PRO_STICK_L_EMULATION_DOWN] = BTN_STICKDOWN;
            profile->Mappings[WPAD_PRO_STICK_L_EMULATION_UP] = BTN_STICKUP;
            break;
        case WPAD_EXT_CLASSIC:
        case WPAD_EXT_MPLUS_CLASSIC:
            profile->Mappings[WPAD_CLASSIC_STICK_R_EMULATION_RIGHT] = BTN_CRIGHT;
            profile->Mappings[WPAD_CLASSIC_STICK_R_EMULATION_LEFT] = BTN_CLEFT;
            profile->Mappings[WPAD_CLASSIC_STICK_R_EMULATION_DOWN] = BTN_CDOWN;
            profile->Mappings[WPAD_CLASSIC_STICK_R_EMULATION_UP] = BTN_CUP;
            profile->Mappings[WPAD_CLASSIC_BUTTON_ZR] = BTN_R;
            profile->Mappings[WPAD_CLASSIC_BUTTON_L] = BTN_L;
            profile->Mappings[WPAD_CLASSIC_BUTTON_RIGHT] = BTN_DRIGHT;
            profile->Mappings[WPAD_CLASSIC_BUTTON_LEFT] = BTN_DLEFT;
            profile->Mappings[WPAD_CLASSIC_BUTTON_DOWN] = BTN_DDOWN;
            profile->Mappings[WPAD_CLASSIC_BUTTON_UP] = BTN_DUP;
            profile->Mappings[WPAD_CLASSIC_BUTTON_PLUS] = BTN_START;
            profile->Mappings[WPAD_CLASSIC_BUTTON_ZL] = BTN_Z;
            profile->Mappings[WPAD_CLASSIC_BUTTON_B] = BTN_B;
            profile->Mappings[WPAD_CLASSIC_BUTTON_A] = BTN_A;
            profile->Mappings[WPAD_CLASSIC_STICK_L_EMULATION_RIGHT] = BTN_STICKRIGHT;
            profile->Mappings[WPAD_CLASSIC_STICK_L_EMULATION_LEFT] = BTN_STICKLEFT;
            profile->Mappings[WPAD_CLASSIC_STICK_L_EMULATION_DOWN] = BTN_STICKDOWN;
            profile->Mappings[WPAD_CLASSIC_STICK_L_EMULATION_UP] = BTN_STICKUP;
            break;
        case WPAD_EXT_NUNCHUK:
        case WPAD_EXT_MPLUS_NUNCHUK:
        case WPAD_EXT_MPLUS:
        case WPAD_EXT_CORE:
            profile->Mappings[WPAD_BUTTON_1] = BTN_R;
            profile->Mappings[WPAD_BUTTON_2] = BTN_L;
            profile->Mappings[WPAD_BUTTON_RIGHT] = BTN_DRIGHT;
            profile->Mappings[WPAD_BUTTON_LEFT] = BTN_DLEFT;
            profile->Mappings[WPAD_BUTTON_DOWN] = BTN_DDOWN;
            profile->Mappings[WPAD_BUTTON_UP] = BTN_DUP;
            profile->Mappings[WPAD_BUTTON_PLUS] = BTN_START;
            profile->Mappings[WPAD_BUTTON_MINUS] = BTN_Z;
            profile->Mappings[WPAD_BUTTON_B] = BTN_B;
            profile->Mappings[WPAD_BUTTON_A] = BTN_A;
            break;
    }

    for (int i = 0; i < 4; i++) {
        profile->AxisDeadzones[i] = 0.0f;
        profile->AxisMinimumPress[i] = 7680.0f;
    }
}

std::string WiiUController::GetControllerExtensionName() {
    switch (mExtensionType) {
        case WPAD_EXT_PRO_CONTROLLER:
            return "ProController";
        case WPAD_EXT_CLASSIC:
        case WPAD_EXT_MPLUS_CLASSIC:
            return "ClassicController";
        case WPAD_EXT_NUNCHUK:
        case WPAD_EXT_MPLUS_NUNCHUK:
        case WPAD_EXT_MPLUS:
        case WPAD_EXT_CORE:
            return "WiiRemote";
    }

    return "Controller";
}

bool WiiUController::Connected() const {
    return mConnected;
};

bool WiiUController::CanGyro() const {
    return false;
}

bool WiiUController::CanRumble() const {
    return true;
};

bool WiiUController::CanSetLed() const {
    return false;
}
} // namespace LUS
#endif
