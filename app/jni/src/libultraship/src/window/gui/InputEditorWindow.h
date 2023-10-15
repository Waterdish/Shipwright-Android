#pragma once

#include "stdint.h"
#include "window/gui/GuiWindow.h"
#include <ImGui/imgui.h>

namespace LUS {

class InputEditorWindow : public GuiWindow {
  public:
    using GuiWindow::GuiWindow;
    ~InputEditorWindow();

    void DrawButton(const char* label, int32_t n64Btn, int32_t currentPort, int32_t* btnReading);
    void DrawControllerSelect(int32_t currentPort);
    void DrawVirtualStick(const char* label, ImVec2 stick, float deadzone = 0);
    void DrawControllerSchema();

  protected:
    void InitElement() override;
    void DrawElement() override;
    void UpdateElement() override;

    int32_t mCurrentPort;

  private:
    int32_t mBtnReading;
    int32_t mGameInputBlockTimer;
    const int32_t mGameInputBlockId = -1;
};
} // namespace LUS
