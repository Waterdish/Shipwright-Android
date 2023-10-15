#include "ControlDeck.h"

#include "Context.h"
#include "Controller.h"
#include "DummyController.h"
#include <Utils/StringHelper.h>
#include "public/bridge/consolevariablebridge.h"
#include <imgui.h>

#ifndef __WIIU__
#include "controller/KeyboardController.h"
#include "controller/SDLController.h"
#else
#include "port/wiiu/WiiUGamepad.h"
#include "port/wiiu/WiiUController.h"
#endif

namespace LUS {

ControlDeck::ControlDeck() : mPads(nullptr) {
}

ControlDeck::~ControlDeck() {
    SPDLOG_TRACE("destruct control deck");
}

void ControlDeck::Init(uint8_t* bits) {
    ScanDevices();
    mControllerBits = bits;
}

void ControlDeck::ScanDevices() {
    mPortList.clear();
    mDevices.clear();

    // Always load controllers that need their device indices zero based first because we add some other devices
    // afterward.
    int32_t i;

#ifndef __WIIU__
    for (i = 0; i < SDL_NumJoysticks(); i++) {
        if (SDL_IsGameController(i)) {
            auto sdl = std::make_shared<SDLController>(i);
            sdl->Open();
            mDevices.push_back(sdl);
        }
    }

    mDevices.push_back(std::make_shared<DummyController>(i++, "Auto", "Auto", true));
    mDevices.push_back(std::make_shared<KeyboardController>(i++));
#else
    for (i = 0; i < 4; i++) {
        auto controller = std::make_shared<LUS::WiiUController>(i, (WPADChan)i);
        controller->Open();
        mDevices.push_back(controller);
    }

    auto gamepad = std::make_shared<LUS::WiiUGamepad>(i++);
    gamepad->Open();
    mDevices.push_back(gamepad);

    mDevices.push_back(std::make_shared<DummyController>(i++, "Auto", "Auto", true));
#endif

    mDevices.push_back(std::make_shared<DummyController>(i++, "Disconnected", "None", false));

    for (const auto& device : mDevices) {
        for (int32_t i = 0; i < MAXCONTROLLERS; i++) {
            device->CreateDefaultBinding(i);
        }
    }

    for (int32_t i = 0; i < MAXCONTROLLERS; i++) {
        mPortList.push_back(i == 0 ? 0 : static_cast<int>(mDevices.size()) - 1);
    }

    LoadSettings();
}

void ControlDeck::SetDeviceToPort(int32_t portIndex, int32_t deviceIndex) {
    const std::shared_ptr<Controller> backend = mDevices[deviceIndex];
    mPortList[portIndex] = deviceIndex;
    *mControllerBits |= (backend->Connected()) << portIndex;
}

void ControlDeck::WriteToPad(OSContPad* pad) {
    mPads = pad;

    for (size_t i = 0; i < mPortList.size(); i++) {
        const std::shared_ptr<Controller> backend = mDevices[mPortList[i]];

        // If the controller backend is "Auto" we need to get the real device
        // we search for the real device to read input from it
        if (backend->GetGuid() == "Auto") {
            for (const auto& device : mDevices) {
                if (IsBlockingGameInput(device->GetGuid())) {
                    device->ReadToPad(nullptr, i);
                    continue;
                }

                device->ReadToPad(&pad[i], i);
            }
            continue;
        }

        if (IsBlockingGameInput(backend->GetGuid())) {
            backend->ReadToPad(nullptr, i);
            continue;
        }

        backend->ReadToPad(&pad[i], i);
    }
}

OSContPad* ControlDeck::GetPads() {
    return mPads;
}

#define NESTED(key, ...) \
    StringHelper::Sprintf("Controllers.%s.Slot_%d." key, device->GetGuid().c_str(), virtualSlot, __VA_ARGS__)

void ControlDeck::LoadSettings() {
    std::shared_ptr<Config> config = Context::GetInstance()->GetConfig();

    auto json = config->GetNestedJson();
    for (auto const& val : json["Controllers"]["Deck"].items()) {
        int32_t slot = std::stoi(val.key().substr(5));

        for (size_t dev = 0; dev < mDevices.size(); dev++) {
            std::string guid = mDevices[dev]->GetGuid();
            if (guid != val.value().get<std::string>()) {
                continue;
            }

            mPortList[slot] = dev;
        }
    }

    for (size_t i = 0; i < mPortList.size(); i++) {
        std::shared_ptr<Controller> backend = mDevices[mPortList[i]];
        config->SetString(StringHelper::Sprintf("Controllers.Deck.Slot_%d", (int32_t)i), backend->GetGuid());
    }

    for (const auto& device : mDevices) {
        std::string guid = device->GetGuid();

        for (int32_t virtualSlot = 0; virtualSlot < MAXCONTROLLERS; virtualSlot++) {

            if (!(config->GetNestedJson()["Controllers"].contains(guid) &&
                  config->GetNestedJson()["Controllers"][guid].contains(
                      StringHelper::Sprintf("Slot_%d", virtualSlot)))) {
                continue;
            }

            auto profile = device->GetProfile(virtualSlot);
            auto rawProfile =
                config->GetNestedJson()["Controllers"][guid][StringHelper::Sprintf("Slot_%d", virtualSlot)];

            profile->Mappings.clear();
            profile->AxisDeadzones.clear();
            profile->AxisMinimumPress.clear();
            profile->GyroData.clear();

            profile->Version = config->GetInt(NESTED("Version", ""), DEVICE_PROFILE_VERSION_0);

            switch (profile->Version) {

                case DEVICE_PROFILE_VERSION_0:

                    // Load up defaults for the things we can't load.
                    device->CreateDefaultBinding(virtualSlot);

                    profile->UseRumble = config->GetBool(NESTED("Rumble.Enabled", ""));
                    profile->RumbleStrength = config->GetFloat(NESTED("Rumble.Strength", ""));
                    profile->UseGyro = config->GetBool(NESTED("Gyro.Enabled", ""));

                    for (auto const& val : rawProfile["Mappings"].items()) {
                        device->SetButtonMapping(virtualSlot, val.value(), std::stoi(val.key().substr(4)));
                    }

                    break;

                case DEVICE_PROFILE_VERSION_1:
                    profile->UseRumble = config->GetBool(NESTED("Rumble.Enabled", ""));
                    profile->RumbleStrength = config->GetFloat(NESTED("Rumble.Strength", ""));
                    profile->UseGyro = config->GetBool(NESTED("Gyro.Enabled", ""));
                    profile->NotchProximityThreshold = config->GetInt(NESTED("Notches.ProximityThreshold", ""));

                    for (auto const& val : rawProfile["AxisDeadzones"].items()) {
                        profile->AxisDeadzones[std::stoi(val.key())] = val.value();
                    }

                    for (auto const& val : rawProfile["AxisMinimumPress"].items()) {
                        profile->AxisMinimumPress[std::stoi(val.key())] = val.value();
                    }

                    for (auto const& val : rawProfile["GyroData"].items()) {
                        profile->GyroData[std::stoi(val.key())] = val.value();
                    }

                    for (auto const& val : rawProfile["Mappings"].items()) {
                        device->SetButtonMapping(virtualSlot, val.value(), std::stoi(val.key().substr(4)));
                    }

                    break;

                case DEVICE_PROFILE_VERSION_2:
                    profile->UseRumble = config->GetBool(NESTED("Rumble.Enabled", ""));
                    profile->RumbleStrength = config->GetFloat(NESTED("Rumble.Strength", ""));
                    profile->UseGyro = config->GetBool(NESTED("Gyro.Enabled", ""));
                    profile->NotchProximityThreshold = config->GetInt(NESTED("Notches.ProximityThreshold", ""));
                    profile->UseStickDeadzoneForButtons = config->GetBool(NESTED("UseStickDeadzoneForButtons", ""));

                    for (auto const& val : rawProfile["AxisDeadzones"].items()) {
                        profile->AxisDeadzones[std::stoi(val.key())] = val.value();
                    }

                    for (auto const& val : rawProfile["AxisMinimumPress"].items()) {
                        profile->AxisMinimumPress[std::stoi(val.key())] = val.value();
                    }

                    for (auto const& val : rawProfile["GyroData"].items()) {
                        profile->GyroData[std::stoi(val.key())] = val.value();
                    }

                    for (auto const& val : rawProfile["Mappings"].items()) {
                        device->SetButtonMapping(virtualSlot, std::stoi(val.key()), val.value());
                    }

                    break;

                // Version is invalid.
                default:
                    device->CreateDefaultBinding(virtualSlot);
                    break;
            }
        }
    }
}

void ControlDeck::SaveSettings() {
    std::shared_ptr<Config> config = Context::GetInstance()->GetConfig();

    for (size_t i = 0; i < mPortList.size(); i++) {
        std::shared_ptr<Controller> backend = mDevices[mPortList[i]];
        config->SetString(StringHelper::Sprintf("Controllers.Deck.Slot_%d", (int32_t)i), backend->GetGuid());
    }

    for (const auto& device : mDevices) {
        std::string guid = device->GetGuid();

        for (int32_t virtualSlot = 0; virtualSlot < MAXCONTROLLERS; virtualSlot++) {
            auto profile = device->GetProfile(virtualSlot);

            if (!device->Connected()) {
                continue;
            }

            // We always save to the most recent version.
            profile->Version = DEVICE_PROFILE_CURRENT_VERSION;

            auto conf = config->GetNestedJson()["Controllers"][guid][StringHelper::Sprintf("Slot_%d", virtualSlot)];

            config->SetInt(NESTED("Version", ""), profile->Version);
            config->SetBool(NESTED("Rumble.Enabled", ""), profile->UseRumble);
            config->SetFloat(NESTED("Rumble.Strength", ""), profile->RumbleStrength);
            config->SetBool(NESTED("Gyro.Enabled", ""), profile->UseGyro);
            config->SetInt(NESTED("Notches.ProximityThreshold", ""), profile->NotchProximityThreshold);
            config->SetBool(NESTED("UseStickDeadzoneForButtons", ""), profile->UseStickDeadzoneForButtons);

            // Clear all sections with a one controller to many relationship.
            const static std::vector<std::string> sClearSections = { "Mappings", "AxisDeadzones", "AxisMinimumPress",
                                                                     "GyroData" };
            for (auto const& section : sClearSections) {
                if (conf.contains(section)) {
                    for (auto const& val : conf[section].items()) {
                        config->Erase(NESTED("%s.%s", section.c_str(), val.key().c_str()));
                    }
                }
            }

            for (auto const& [key, val] : profile->Mappings) {
                config->SetInt(NESTED("Mappings.%d", key), val);
            }

            for (auto const& [key, val] : profile->AxisDeadzones) {
                config->SetFloat(NESTED("AxisDeadzones.%d", key), val);
            }

            for (auto const& [key, val] : profile->AxisMinimumPress) {
                config->SetFloat(NESTED("AxisMinimumPress.%d", key), val);
            }

            for (auto const& [key, val] : profile->GyroData) {
                config->SetFloat(NESTED("GyroData.%d", key), val);
            }
        }
    }

    config->Save();
}

std::shared_ptr<Controller> ControlDeck::GetDeviceFromDeviceIndex(int32_t deviceIndex) {
    return mDevices[deviceIndex];
}

size_t ControlDeck::GetNumDevices() {
    return mDevices.size();
}

int32_t ControlDeck::GetDeviceIndexFromPortIndex(int32_t portIndex) {
    return mPortList[portIndex];
}

size_t ControlDeck::GetNumConnectedPorts() {
    return mPortList.size();
}

std::shared_ptr<Controller> ControlDeck::GetDeviceFromPortIndex(int32_t portIndex) {
    return GetDeviceFromDeviceIndex(GetDeviceIndexFromPortIndex(portIndex));
}

uint8_t* ControlDeck::GetControllerBits() {
    return mControllerBits;
}

void ControlDeck::BlockGameInput(int32_t inputBlockId) {
    mGameInputBlockers[inputBlockId] = true;
}

void ControlDeck::UnblockGameInput(int32_t inputBlockId) {
    mGameInputBlockers.erase(inputBlockId);
}

bool ControlDeck::IsBlockingGameInput(const std::string& inputDeviceGuid) const {
    // We block controller input if F1 menu is open and control navigation is on.
    // This is because we don't want controller inputs to affect the game
    bool shouldBlockControllerInput = CVarGetInteger("gOpenMenuBar", 0) && CVarGetInteger("gControlNav", 0);

    // We block keyboard input if you're currently typing into a textfield.
    // This is because we don't want your keyboard typing to affect the game.
    ImGuiIO io = ImGui::GetIO();
    bool shouldBlockKeyboardInput = io.WantCaptureKeyboard;

    bool inputDeviceIsKeyboard = inputDeviceGuid == "Keyboard";
    return (!mGameInputBlockers.empty()) ||
           (inputDeviceIsKeyboard ? shouldBlockKeyboardInput : shouldBlockControllerInput);
}
} // namespace LUS
