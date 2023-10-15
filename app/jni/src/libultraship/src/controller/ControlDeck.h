#pragma once

#include "Controller.h"
#include <vector>
#include <config/Config.h>

namespace LUS {

class ControlDeck {
  public:
    ControlDeck();
    ~ControlDeck();

    void Init(uint8_t* controllerBits);
    void ScanDevices();
    void WriteToPad(OSContPad* pad);
    OSContPad* GetPads();
    void LoadSettings();
    void SaveSettings();
    void SetDeviceToPort(int32_t portIndex, int32_t deviceIndex);
    std::shared_ptr<Controller> GetDeviceFromDeviceIndex(int32_t deviceIndex);
    std::shared_ptr<Controller> GetDeviceFromPortIndex(int32_t portIndex);
    int32_t GetDeviceIndexFromPortIndex(int32_t portIndex);
    size_t GetNumDevices();
    size_t GetNumConnectedPorts();
    uint8_t* GetControllerBits();
    void BlockGameInput(int32_t inputBlockId);
    void UnblockGameInput(int32_t inputBlockId);
    bool IsBlockingGameInput(const std::string& inputDeviceGuid) const;

  private:
    std::vector<int32_t> mPortList = {};
    std::vector<std::shared_ptr<Controller>> mDevices = {};
    uint8_t* mControllerBits = nullptr;
    std::unordered_map<int32_t, bool> mGameInputBlockers;
    OSContPad* mPads;
};
} // namespace LUS
