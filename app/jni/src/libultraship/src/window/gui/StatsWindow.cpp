#include "StatsWindow.h"
#include "ImGui/imgui.h"
#include "public/bridge/consolevariablebridge.h"
#include "spdlog/spdlog.h"

namespace LUS {
StatsWindow::~StatsWindow() {
    SPDLOG_TRACE("destruct stats window");
}

void StatsWindow::InitElement() {
}

void StatsWindow::DrawElement() {
    const float framerate = ImGui::GetIO().Framerate;
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));
    ImGui::Begin("Debug Stats", &mIsVisible, ImGuiWindowFlags_NoFocusOnAppearing);

#if defined(_WIN32)
    ImGui::Text("Platform: Windows");
#elif defined(__APPLE__)
    ImGui::Text("Platform: macOS");
#elif defined(__SWITCH__)
    ImGui::Text("Platform: Nintendo Switch");
#elif defined(__WIIU__)
    ImGui::Text("Platform: Nintendo Wii U");
#elif defined(__linux__)
    ImGui::Text("Platform: Linux");
#else
    ImGui::Text("Platform: Unknown");
#endif
    ImGui::Text("Status: %.3f ms/frame (%.1f FPS)", 1000.0f / framerate, framerate);
    ImGui::End();
    ImGui::PopStyleColor();
}

void StatsWindow::UpdateElement() {
}
} // namespace LUS
