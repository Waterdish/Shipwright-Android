#pragma once

#include <string>
#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif
#include <ImGui/imgui.h>
#include <ImGui/imgui_internal.h>
#include "window/gui/GuiElement.h"

namespace LUS {
class GuiWindow : public GuiElement {
  public:
    GuiWindow(const std::string& consoleVariable, bool isVisible, const std::string& name);
    GuiWindow(const std::string& consoleVariable, const std::string& name);

    std::string GetName();

  protected:
    void BeginGroupPanel(const char* name, const ImVec2& size);
    void EndGroupPanel(float minHeight);

  private:
    std::string mName;
    ImVector<ImRect> mGroupPanelLabelStack;
};
} // namespace LUS
