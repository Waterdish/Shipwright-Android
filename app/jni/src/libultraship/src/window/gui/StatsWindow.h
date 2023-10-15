#pragma once

#include "window/gui/GuiWindow.h"

namespace LUS {
class StatsWindow : public GuiWindow {
  public:
    using GuiWindow::GuiWindow;
    ~StatsWindow();

  private:
    void InitElement() override;
    void DrawElement() override;
    void UpdateElement() override;
};
} // namespace LUS