#include "InputEditorWindow.h"
#include "controller/Controller.h"
#include "controller/KeyboardController.h"
#include "Context.h"
#include "Gui.h"
#include <Utils/StringHelper.h>
#include "public/bridge/consolevariablebridge.h"

namespace LUS {

#define SEPARATION() ImGui::Dummy(ImVec2(0, 5))

InputEditorWindow::~InputEditorWindow() {
    SPDLOG_TRACE("destruct input editor window");
}

void InputEditorWindow::InitElement() {
    mCurrentPort = 0;
    mBtnReading = -1;
    mGameInputBlockTimer = INT32_MAX;
}

std::shared_ptr<Controller> GetControllerPerSlot(int slot) {
    auto controlDeck = Context::GetInstance()->GetControlDeck();
    return controlDeck->GetDeviceFromPortIndex(slot);
}

void InputEditorWindow::DrawButton(const char* label, int32_t n64Btn, int32_t currentPort, int32_t* btnReading) {
    const std::shared_ptr<Controller> backend = GetControllerPerSlot(currentPort);

    float size = 40;
    bool readingMode = *btnReading == n64Btn;
    bool disabled = (*btnReading != -1 && !readingMode) || !backend->Connected() || backend->GetGuid() == "Auto";
    ImVec2 len = ImGui::CalcTextSize(label);
    ImVec2 pos = ImGui::GetCursorPos();
    ImGui::SetCursorPosY(pos.y + len.y / 4);
    ImGui::SetCursorPosX(pos.x + abs(len.x - size));
    ImGui::Text("%s", label);
    ImGui::SameLine();
    ImGui::SetCursorPosY(pos.y);

    if (disabled) {
        ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
    }

    if (readingMode) {
        const int32_t btn = backend->ReadRawPress();
        Context::GetInstance()->GetControlDeck()->BlockGameInput(mGameInputBlockId);

        if (btn != -1) {
            auto profile = backend->GetProfile(currentPort);
            // Remove other mappings that include the n64 bitmask. Note that the n64 button is really a mask and is not
            // unique, but the UI as-is needs a way to unset the old n64 button.
            std::erase_if(profile->Mappings,
                          [n64Btn](const std::pair<int32_t, int32_t>& bin) { return bin.second == n64Btn; });
            backend->SetButtonMapping(currentPort, btn, n64Btn);
            auto keyboardBackend = dynamic_pointer_cast<KeyboardController>(backend);
            if (keyboardBackend != nullptr) {
                // Window backend has not called release on the scancode and thus we need to release all buttons.
                keyboardBackend->ReleaseAllButtons();
            }
            Context::GetInstance()->GetControlDeck()->SaveSettings();

            *btnReading = -1;

            // avoid immediately triggering another button during gamepad nav
            ImGui::SetKeyboardFocusHere(0);

            // don't send input to the game for a third of a second after getting the mapping
            mGameInputBlockTimer = ImGui::GetIO().Framerate / 3;
        }
    }

    const std::string btnName = backend->GetButtonName(currentPort, n64Btn);

    if (ImGui::Button(
            StringHelper::Sprintf("%s##HBTNID_%d", readingMode ? "Press a Key..." : btnName.c_str(), n64Btn).c_str())) {
        *btnReading = n64Btn;
        backend->ClearRawPress();
    }

    if (disabled) {
        ImGui::PopItemFlag();
        ImGui::PopStyleVar();
    }
}

void InputEditorWindow::DrawControllerSelect(int32_t currentPort) {
    auto controlDeck = Context::GetInstance()->GetControlDeck();
    std::string controllerName = controlDeck->GetDeviceFromPortIndex(currentPort)->GetControllerName();

    if (ImGui::BeginCombo("##ControllerEntries", controllerName.c_str())) {
        for (uint8_t i = 0; i < controlDeck->GetNumDevices(); i++) {
            std::string deviceName = controlDeck->GetDeviceFromDeviceIndex(i)->GetControllerName();
            if (deviceName != "Keyboard" && deviceName != "Auto") {
                deviceName += "##" + std::to_string(i);
            }
            if (ImGui::Selectable(deviceName.c_str(), i == controlDeck->GetDeviceIndexFromPortIndex(currentPort))) {
                controlDeck->SetDeviceToPort(currentPort, i);
                Context::GetInstance()->GetControlDeck()->SaveSettings();
            }
        }
        ImGui::EndCombo();
    }

    ImGui::SameLine();

    if (ImGui::Button("Refresh")) {
        controlDeck->ScanDevices();
    }
}

void InputEditorWindow::DrawVirtualStick(const char* label, ImVec2 stick, float deadzone) {
    ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x + 5, ImGui::GetCursorPos().y));
    ImGui::BeginChild(label, ImVec2(68, 75), false);
    ImDrawList* drawList = ImGui::GetWindowDrawList();

    const ImVec2 cursorScreenPosition = ImGui::GetCursorScreenPos();

    // Draw the border box
    float borderSquareLeft = cursorScreenPosition.x + 2.0f;
    float borderSquareTop = cursorScreenPosition.y + 2.0f;
    float borderSquareSize = 65.0f;
    drawList->AddRect(ImVec2(borderSquareLeft, borderSquareTop),
                      ImVec2(borderSquareLeft + borderSquareSize, borderSquareTop + borderSquareSize),
                      ImColor(100, 100, 100, 255), 0.0f, 0, 1.5f);

    // Draw the gate background
    float cardinalRadius = 22.5f;
    float diagonalRadius = cardinalRadius * (69.0f / 85.0f);

    ImVec2 joystickCenterpoint =
        ImVec2(cursorScreenPosition.x + cardinalRadius + 12, cursorScreenPosition.y + cardinalRadius + 11);
    drawList->AddQuadFilled(joystickCenterpoint,
                            ImVec2(joystickCenterpoint.x - diagonalRadius, joystickCenterpoint.y + diagonalRadius),
                            ImVec2(joystickCenterpoint.x, joystickCenterpoint.y + cardinalRadius),
                            ImVec2(joystickCenterpoint.x + diagonalRadius, joystickCenterpoint.y + diagonalRadius),
                            ImColor(130, 130, 130, 255));
    drawList->AddQuadFilled(joystickCenterpoint,
                            ImVec2(joystickCenterpoint.x + diagonalRadius, joystickCenterpoint.y + diagonalRadius),
                            ImVec2(joystickCenterpoint.x + cardinalRadius, joystickCenterpoint.y),
                            ImVec2(joystickCenterpoint.x + diagonalRadius, joystickCenterpoint.y - diagonalRadius),
                            ImColor(130, 130, 130, 255));
    drawList->AddQuadFilled(joystickCenterpoint,
                            ImVec2(joystickCenterpoint.x + diagonalRadius, joystickCenterpoint.y - diagonalRadius),
                            ImVec2(joystickCenterpoint.x, joystickCenterpoint.y - cardinalRadius),
                            ImVec2(joystickCenterpoint.x - diagonalRadius, joystickCenterpoint.y - diagonalRadius),
                            ImColor(130, 130, 130, 255));
    drawList->AddQuadFilled(joystickCenterpoint,
                            ImVec2(joystickCenterpoint.x - diagonalRadius, joystickCenterpoint.y - diagonalRadius),
                            ImVec2(joystickCenterpoint.x - cardinalRadius, joystickCenterpoint.y),
                            ImVec2(joystickCenterpoint.x - diagonalRadius, joystickCenterpoint.y + diagonalRadius),
                            ImColor(130, 130, 130, 255));

    // Draw the joystick position indicator
    ImVec2 joystickIndicatorDistanceFromCenter = ImVec2(0, 0);
    if ((stick.x * stick.x + stick.y * stick.y) > (deadzone * deadzone)) {
        joystickIndicatorDistanceFromCenter =
            ImVec2((stick.x * (cardinalRadius / 85.0f)), -(stick.y * (cardinalRadius / 85.0f)));
    }
    float indicatorRadius = 5.0f;
    drawList->AddCircleFilled(ImVec2(joystickCenterpoint.x + joystickIndicatorDistanceFromCenter.x,
                                     joystickCenterpoint.y + joystickIndicatorDistanceFromCenter.y),
                              indicatorRadius, ImColor(34, 51, 76, 255), 7);

    ImGui::EndChild();
}

void InputEditorWindow::DrawControllerSchema() {
    auto backend = Context::GetInstance()->GetControlDeck()->GetDeviceFromPortIndex(mCurrentPort);
    auto profile = backend->GetProfile(mCurrentPort);
    bool isKeyboard = backend->GetGuid() == "Keyboard" || backend->GetGuid() == "Auto" || !backend->Connected();

    backend->ReadToPad(nullptr, mCurrentPort);

    DrawControllerSelect(mCurrentPort);

    BeginGroupPanel("Buttons", ImVec2(150, 20));
    DrawButton("A", BTN_A, mCurrentPort, &mBtnReading);
    DrawButton("B", BTN_B, mCurrentPort, &mBtnReading);
    DrawButton("L", BTN_L, mCurrentPort, &mBtnReading);
    DrawButton("R", BTN_R, mCurrentPort, &mBtnReading);
    DrawButton("Z", BTN_Z, mCurrentPort, &mBtnReading);
    DrawButton("START", BTN_START, mCurrentPort, &mBtnReading);
    SEPARATION();
#ifdef __SWITCH__
    EndGroupPanel(isKeyboard ? 7.0f : 56.0f);
#else
    EndGroupPanel(isKeyboard ? 7.0f : 48.0f);
#endif
    ImGui::SameLine();
    BeginGroupPanel("Digital Pad", ImVec2(150, 20));
    DrawButton("Up", BTN_DUP, mCurrentPort, &mBtnReading);
    DrawButton("Down", BTN_DDOWN, mCurrentPort, &mBtnReading);
    DrawButton("Left", BTN_DLEFT, mCurrentPort, &mBtnReading);
    DrawButton("Right", BTN_DRIGHT, mCurrentPort, &mBtnReading);
    SEPARATION();
#ifdef __SWITCH__
    EndGroupPanel(isKeyboard ? 53.0f : 122.0f);
#else
    EndGroupPanel(isKeyboard ? 53.0f : 94.0f);
#endif
    ImGui::SameLine();
    BeginGroupPanel("Analog Stick", ImVec2(150, 20));
    DrawButton("Up", BTN_STICKUP, mCurrentPort, &mBtnReading);
    DrawButton("Down", BTN_STICKDOWN, mCurrentPort, &mBtnReading);
    DrawButton("Left", BTN_STICKLEFT, mCurrentPort, &mBtnReading);
    DrawButton("Right", BTN_STICKRIGHT, mCurrentPort, &mBtnReading);

    if (!isKeyboard) {
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 8);
        DrawVirtualStick("##MainVirtualStick",
                         ImVec2(backend->GetLeftStickX(mCurrentPort), backend->GetLeftStickY(mCurrentPort)),
                         profile->AxisDeadzones[0]);
        ImGui::SameLine();

        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 5);

#ifdef __WIIU__
        ImGui::BeginChild("##MSInput", ImVec2(90 * 2, 50 * 2), false);
#else
        ImGui::BeginChild("##MSInput", ImVec2(90, 50), false);
#endif
        ImGui::Text("Deadzone");
#ifdef __WIIU__
        ImGui::PushItemWidth(80 * 2);
#else
        ImGui::PushItemWidth(80);
#endif
        // The window has deadzone per stick, so we need to
        // set the deadzone for both left stick axes here
        // SDL_CONTROLLER_AXIS_LEFTX: 0
        // SDL_CONTROLLER_AXIS_LEFTY: 1
        if (ImGui::InputFloat("##MDZone", &profile->AxisDeadzones[0], 1.0f, 0.0f, "%.0f")) {
            if (profile->AxisDeadzones[0] < 0) {
                profile->AxisDeadzones[0] = 0;
            }
            profile->AxisDeadzones[1] = profile->AxisDeadzones[0];
            Context::GetInstance()->GetControlDeck()->SaveSettings();
        }
        ImGui::PopItemWidth();
        ImGui::EndChild();
    } else {
        ImGui::Dummy(ImVec2(0, 6));
    }
#ifdef __SWITCH__
    EndGroupPanel(isKeyboard ? 52.0f : 52.0f);
#else
    EndGroupPanel(isKeyboard ? 52.0f : 24.0f);
#endif
    ImGui::SameLine();

    if (!isKeyboard) {
        ImGui::SameLine();
        BeginGroupPanel("Right Stick", ImVec2(150, 20));
        DrawButton("Up", BTN_VSTICKUP, mCurrentPort, &mBtnReading);
        DrawButton("Down", BTN_VSTICKDOWN, mCurrentPort, &mBtnReading);
        DrawButton("Left", BTN_VSTICKLEFT, mCurrentPort, &mBtnReading);
        DrawButton("Right", BTN_VSTICKRIGHT, mCurrentPort, &mBtnReading);

        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 8);
        // 2 is the SDL value for right stick X axis
        // 3 is the SDL value for right stick Y axis.
        DrawVirtualStick("##RightVirtualStick",
                         ImVec2(backend->GetRightStickX(mCurrentPort), backend->GetRightStickY(mCurrentPort)),
                         profile->AxisDeadzones[2]);

        ImGui::SameLine();
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 5);
#ifdef __WIIU__
        ImGui::BeginChild("##CSInput", ImVec2(90 * 2, 85 * 2), false);
#else
        ImGui::BeginChild("##CSInput", ImVec2(90, 85), false);
#endif
        ImGui::Text("Deadzone");
#ifdef __WIIU__
        ImGui::PushItemWidth(80 * 2);
#else
        ImGui::PushItemWidth(80);
#endif
        // The window has deadzone per stick, so we need to
        // set the deadzone for both right stick axes here
        // SDL_CONTROLLER_AXIS_RIGHTX: 2
        // SDL_CONTROLLER_AXIS_RIGHTY: 3
        if (ImGui::InputFloat("##MDZone", &profile->AxisDeadzones[2], 1.0f, 0.0f, "%.0f")) {
            if (profile->AxisDeadzones[2] < 0) {
                profile->AxisDeadzones[2] = 0;
            }
            Context::GetInstance()->GetControlDeck()->SaveSettings();
            profile->AxisDeadzones[3] = profile->AxisDeadzones[2];
        }
        ImGui::PopItemWidth();
        ImGui::EndChild();
#ifdef __SWITCH__
        EndGroupPanel(43.0f);
#else
        EndGroupPanel(14.0f);
#endif
    }

    if (backend->CanGyro()) {
#ifndef __WIIU__
        ImGui::SameLine();
#endif
        BeginGroupPanel("Gyro Options", ImVec2(175, 20));
        float cursorX = ImGui::GetCursorPosX() + 5;
        ImGui::SetCursorPosX(cursorX);
        if (ImGui::Checkbox("Enable Gyro", &profile->UseGyro)) {
            Context::GetInstance()->GetControlDeck()->SaveSettings();
        }
        ImGui::SetCursorPosX(cursorX);
        ImGui::Text("Gyro Sensitivity: %d%%", static_cast<int>(100.0f * profile->GyroData[GYRO_SENSITIVITY]));
#ifdef __WIIU__
        ImGui::PushItemWidth(135.0f * 2);
#else
        ImGui::PushItemWidth(135.0f);
#endif
        ImGui::SetCursorPosX(cursorX);
        if (ImGui::SliderFloat("##GSensitivity", &profile->GyroData[GYRO_SENSITIVITY], 0.0f, 1.0f, "")) {
            Context::GetInstance()->GetControlDeck()->SaveSettings();
        }
        ImGui::PopItemWidth();
        ImGui::Dummy(ImVec2(0, 1));
        ImGui::SetCursorPosX(cursorX);
        if (ImGui::Button("Recalibrate Gyro##RGyro")) {
            profile->GyroData[DRIFT_X] = 0.0f;
            profile->GyroData[DRIFT_Y] = 0.0f;
            Context::GetInstance()->GetControlDeck()->SaveSettings();
        }
        ImGui::SetCursorPosX(cursorX);
        DrawVirtualStick("##GyroPreview",
                         ImVec2(-10.0f * backend->GetGyroY(mCurrentPort), 10.0f * backend->GetGyroX(mCurrentPort)));

        ImGui::SameLine();
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 5);
#ifdef __WIIU__
        ImGui::BeginChild("##GyInput", ImVec2(90 * 2, 85 * 2), false);
#else
        ImGui::BeginChild("##GyInput", ImVec2(90, 85), false);
#endif
        ImGui::Text("Drift X");
#ifdef __WIIU__
        ImGui::PushItemWidth(80 * 2);
#else
        ImGui::PushItemWidth(80);
#endif
        if (ImGui::InputFloat("##GDriftX", &profile->GyroData[DRIFT_X], 1.0f, 0.0f, "%.1f")) {
            Context::GetInstance()->GetControlDeck()->SaveSettings();
        }
        ImGui::PopItemWidth();
        ImGui::Text("Drift Y");
#ifdef __WIIU__
        ImGui::PushItemWidth(80 * 2);
#else
        ImGui::PushItemWidth(80);
#endif
        if (ImGui::InputFloat("##GDriftY", &profile->GyroData[DRIFT_Y], 1.0f, 0.0f, "%.1f")) {
            Context::GetInstance()->GetControlDeck()->SaveSettings();
        }
        ImGui::PopItemWidth();
        ImGui::EndChild();
#ifdef __SWITCH__
        EndGroupPanel(46.0f);
#else
        EndGroupPanel(14.0f);
#endif
    }

    ImGui::SameLine();

    const ImVec2 cursor = ImGui::GetCursorPos();

    BeginGroupPanel("C-Buttons", ImVec2(158, 20));
    DrawButton("Up", BTN_CUP, mCurrentPort, &mBtnReading);
    DrawButton("Down", BTN_CDOWN, mCurrentPort, &mBtnReading);
    DrawButton("Left", BTN_CLEFT, mCurrentPort, &mBtnReading);
    DrawButton("Right", BTN_CRIGHT, mCurrentPort, &mBtnReading);
    ImGui::Dummy(ImVec2(0, 5));
#ifdef __SWITCH__
    EndGroupPanel(isKeyboard ? 53.0f : 122.0f);
#else
    EndGroupPanel(isKeyboard ? 53.0f : 94.0f);
#endif

    ImGui::SetCursorPosX(cursor.x);
    ImGui::SameLine();

    BeginGroupPanel("Options", ImVec2(158, 20));
    float cursorX = ImGui::GetCursorPosX() + 5;
    ImGui::SetCursorPosX(cursorX);
    if (ImGui::Checkbox("Rumble Enabled", &profile->UseRumble)) {
        Context::GetInstance()->GetControlDeck()->SaveSettings();
    }
    if (backend->CanRumble()) {
        ImGui::SetCursorPosX(cursorX);
        ImGui::Text("Rumble Force: %d%%", static_cast<int32_t>(100.0f * profile->RumbleStrength));
        ImGui::SetCursorPosX(cursorX);
#ifdef __WIIU__
        ImGui::PushItemWidth(135.0f * 2);
#else
        ImGui::PushItemWidth(135.0f);
#endif
        if (ImGui::SliderFloat("##RStrength", &profile->RumbleStrength, 0.0f, 1.0f, "")) {
            Context::GetInstance()->GetControlDeck()->SaveSettings();
        }
        ImGui::PopItemWidth();
    }

    if (!isKeyboard) {
        ImGui::SetCursorPosX(cursorX);
        ImGui::Text("Notch Snap Angle: %d", profile->NotchProximityThreshold);
        ImGui::SetCursorPosX(cursorX);

#ifdef __WIIU__
        ImGui::PushItemWidth(135.0f * 2);
#else
        ImGui::PushItemWidth(135.0f);
#endif
        if (ImGui::SliderInt("##NotchProximityThreshold", &profile->NotchProximityThreshold, 0, 45, "",
                             ImGuiSliderFlags_AlwaysClamp)) {
            Context::GetInstance()->GetControlDeck()->SaveSettings();
        }
        ImGui::PopItemWidth();
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip(
                "%s", "How near in degrees to a virtual notch angle you have to be for it to snap to nearest notch");
        }

        if (ImGui::Checkbox("Stick Deadzones For Buttons", &profile->UseStickDeadzoneForButtons)) {
            Context::GetInstance()->GetControlDeck()->SaveSettings();
        }
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("%s", "Uses the stick deadzone values when sticks are mapped to buttons");
        }
    }

    ImGui::Dummy(ImVec2(0, 5));
    EndGroupPanel(isKeyboard ? 0.0f : 2.0f);
}

void InputEditorWindow::UpdateElement() {
    if (mGameInputBlockTimer != INT32_MAX) {
        mGameInputBlockTimer--;
        if (mGameInputBlockTimer <= 0) {
            LUS::Context::GetInstance()->GetControlDeck()->UnblockGameInput(mGameInputBlockId);
            mGameInputBlockTimer = INT32_MAX;
        }
    }
}

void InputEditorWindow::DrawElement() {
#ifdef __SWITCH__
    ImVec2 minSize = ImVec2(641, 250);
    ImVec2 maxSize = ImVec2(2200, 505);
#elif defined(__WIIU__)
    ImVec2 minSize = ImVec2(641 * 2, 250 * 2);
    ImVec2 maxSize = ImVec2(1200 * 2, 330 * 2);
#else
    ImVec2 minSize = ImVec2(641, 250);
    ImVec2 maxSize = ImVec2(1200, 330);
#endif

    ImGui::SetNextWindowSizeConstraints(minSize, maxSize);
    // OTRTODO: Disable this stupid workaround ( ReadRawPress() only works when the window is on the main viewport )
    ImGui::SetNextWindowViewport(ImGui::GetMainViewport()->ID);
    ImGui::Begin("Controller Configuration", &mIsVisible,
                 ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize);

    ImGui::BeginTabBar("##Controllers");

    for (int i = 0; i < 4; i++) {
        if (ImGui::BeginTabItem(StringHelper::Sprintf("Port %d", i + 1).c_str())) {
            mCurrentPort = i;
            ImGui::EndTabItem();
        }
    }

    ImGui::EndTabBar();

    // Draw current cfg
    DrawControllerSchema();

    ImGui::End();
}
} // namespace LUS
