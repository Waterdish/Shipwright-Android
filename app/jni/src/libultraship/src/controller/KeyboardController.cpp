#include "controller/KeyboardController.h"
#include "Context.h"

#if __APPLE__
#include <SDL_keyboard.h>
#else
#include <SDL2/SDL_keyboard.h>
#endif

namespace LUS {

KeyboardController::KeyboardController(int32_t deviceIndex) : Controller(deviceIndex), mLastScancode(-1) {
    mGuid = "Keyboard";
    mControllerName = "Keyboard";
}

bool KeyboardController::PressButton(int32_t scancode) {
    mLastKey = scancode;
    bool readSuccess = false;

    for (int32_t portIndex = 0; portIndex < MAXCONTROLLERS; portIndex++) {

        if (GetProfile(portIndex)->Mappings.contains(scancode)) {
            GetPressedButtons(portIndex) |= GetProfile(portIndex)->Mappings[scancode];
            readSuccess = true;
        }
    }

    return readSuccess;
}

bool KeyboardController::ReleaseButton(int32_t scancode) {
    bool readSuccess = false;

    for (int32_t portIndex = 0; portIndex < MAXCONTROLLERS; portIndex++) {
        if (GetProfile(portIndex)->Mappings.contains(scancode)) {
            GetPressedButtons(portIndex) &= ~GetProfile(portIndex)->Mappings[scancode];
            readSuccess = true;
        }
    }

    return readSuccess;
}

void KeyboardController::ReleaseAllButtons() {
    for (int32_t portIndex = 0; portIndex < MAXCONTROLLERS; portIndex++) {
        GetPressedButtons(portIndex) = 0;
    }
}

void KeyboardController::ReadDevice(int32_t portIndex) {
    GetLeftStickX(portIndex) = 0;
    GetLeftStickY(portIndex) = 0;
    GetRightStickX(portIndex) = 0;
    GetRightStickY(portIndex) = 0;
}

int32_t KeyboardController::ReadRawPress() {
    return mLastKey;
}

const std::string KeyboardController::GetButtonName(int32_t portIndex, int32_t n64bitmask) {
    std::map<int32_t, int32_t>& mappings = GetProfile(portIndex)->Mappings;
    // OTRTODO: This should get the scancode of all bits in the mask.
    const auto find =
        std::find_if(mappings.begin(), mappings.end(),
                     [n64bitmask](const std::pair<int32_t, int32_t>& pair) { return pair.second == n64bitmask; });

    if (find == mappings.end()) {
        return "Unknown";
    }
    const char* name = Context::GetInstance()->GetWindow()->GetKeyName(find->first);
    return strlen(name) == 0 ? "Unknown" : name;
}

void KeyboardController::CreateDefaultBinding(int32_t portIndex) {
    auto profile = GetProfile(portIndex);
    profile->Mappings.clear();
    profile->AxisDeadzones.clear();
    profile->AxisMinimumPress.clear();
    profile->GyroData.clear();

    profile->Version = DEVICE_PROFILE_CURRENT_VERSION;
    profile->UseRumble = false;
    profile->RumbleStrength = 1.0f;
    profile->UseGyro = false;

    profile->Mappings[0x14D] = BTN_CRIGHT;
    profile->Mappings[0x14B] = BTN_CLEFT;
    profile->Mappings[0x150] = BTN_CDOWN;
    profile->Mappings[0x148] = BTN_CUP;
    profile->Mappings[0x13] = BTN_R;
    profile->Mappings[0x12] = BTN_L;
    profile->Mappings[0x023] = BTN_DRIGHT;
    profile->Mappings[0x021] = BTN_DLEFT;
    profile->Mappings[0x022] = BTN_DDOWN;
    profile->Mappings[0x014] = BTN_DUP;
    profile->Mappings[0x039] = BTN_START;
    profile->Mappings[0x02C] = BTN_Z;
    profile->Mappings[0x02E] = BTN_B;
    profile->Mappings[0x02D] = BTN_A;
    profile->Mappings[0x020] = BTN_STICKRIGHT;
    profile->Mappings[0x01E] = BTN_STICKLEFT;
    profile->Mappings[0x01F] = BTN_STICKDOWN;
    profile->Mappings[0x011] = BTN_STICKUP;
    profile->Mappings[0x02A] = BTN_MODIFIER1;
    profile->Mappings[0x036] = BTN_MODIFIER2;
}

bool KeyboardController::Connected() const {
    return true;
}

bool KeyboardController::CanRumble() const {
    return false;
}

bool KeyboardController::CanGyro() const {
    return false;
}

void KeyboardController::ClearRawPress() {
    mLastKey = -1;
}

void KeyboardController::SetLastScancode(int32_t key) {
    mLastScancode = key;
}

int32_t KeyboardController::GetLastScancode() {
    return mLastScancode;
}

bool KeyboardController::CanSetLed() const {
    return false;
}

int32_t KeyboardController::SetRumble(int32_t portIndex, bool rumble) {
    return -1001;
}

int32_t KeyboardController::SetLedColor(int32_t portIndex, Color_RGB8 color) {
    // Not supported today, but theoretically we could tie into some of the keyboard APIs to set RGB lights.
    return -1001;
}
} // namespace LUS
