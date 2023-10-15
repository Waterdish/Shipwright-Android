#include "GuiElement.h"

#include "libultraship/libultraship.h"

namespace LUS {
GuiElement::GuiElement(const std::string& visibilityConsoleVariable, bool isVisible)
    : mIsInitialized(false), mIsVisible(isVisible), mVisibilityConsoleVariable(visibilityConsoleVariable) {
    if (!mVisibilityConsoleVariable.empty()) {
        mIsVisible = CVarGetInteger(mVisibilityConsoleVariable.c_str(), mIsVisible);
        SyncVisibilityConsoleVariable();
    }
}

GuiElement::GuiElement(const std::string& visibilityConsoleVariable) : GuiElement(visibilityConsoleVariable, false) {
}

GuiElement::GuiElement(bool isVisible) : GuiElement("", isVisible) {
}

GuiElement::GuiElement() : GuiElement("", false) {
}

GuiElement::~GuiElement() {
}

void GuiElement::Init() {
    if (IsInitialized()) {
        return;
    }

    InitElement();
    mIsInitialized = true;
}

void GuiElement::Draw() {
    if (!IsVisible()) {
        return;
    }

    DrawElement();
    // Sync up the IsVisible flag if it was changed by ImGui
    SyncVisibilityConsoleVariable();
}

void GuiElement::Update() {
    UpdateElement();
}

void GuiElement::SyncVisibilityConsoleVariable() {
    if (mVisibilityConsoleVariable.empty()) {
        return;
    }

    bool shouldSave = CVarGetInteger(mVisibilityConsoleVariable.c_str(), 0) != IsVisible();

    if (IsVisible()) {
        CVarSetInteger(mVisibilityConsoleVariable.c_str(), IsVisible());
    } else {
        CVarClear(mVisibilityConsoleVariable.c_str());
    }

    if (shouldSave) {
        LUS::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesOnNextTick();
    }
}

void GuiElement::SetVisiblity(bool visible) {
    mIsVisible = visible;
    SyncVisibilityConsoleVariable();
}

void GuiElement::Show() {
    SetVisiblity(true);
}

void GuiElement::Hide() {
    SetVisiblity(false);
}

void GuiElement::ToggleVisibility() {
    SetVisiblity(!IsVisible());
}

bool GuiElement::IsVisible() {
    return mIsVisible;
}

bool GuiElement::IsInitialized() {
    return mIsInitialized;
}
} // namespace LUS
