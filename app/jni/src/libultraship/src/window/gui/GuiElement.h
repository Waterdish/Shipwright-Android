#pragma once

#include <string>

namespace LUS {
class GuiElement {
  public:
    GuiElement(const std::string& visibilityConsoleVariable, bool isVisible);
    GuiElement(const std::string& visibilityConsoleVariable);
    GuiElement(bool isVisible);
    GuiElement();
    ~GuiElement();

    void Init();
    void Draw();
    void Update();

    void Show();
    void Hide();
    void ToggleVisibility();
    bool IsVisible();
    bool IsInitialized();

  protected:
    virtual void InitElement() = 0;
    virtual void DrawElement() = 0;
    virtual void UpdateElement() = 0;

    void SetVisiblity(bool visible);
    bool mIsVisible;

  private:
    void SyncVisibilityConsoleVariable();
    std::string mVisibilityConsoleVariable;

    bool mIsInitialized;
};

} // namespace LUS
