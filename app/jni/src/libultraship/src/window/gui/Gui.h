#pragma once

#ifdef __cplusplus
#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif
#include <ImGui/imgui.h>
#include <ImGui/imgui_internal.h>
#include <memory>
#include <SDL2/SDL.h>
#include "window/gui/ConsoleWindow.h"
#include "window/gui/InputEditorWindow.h"
#include "window/gui/IconsFontAwesome4.h"
#include "window/gui/GameOverlay.h"
#include "window/gui/InputViewer.h"
#include "window/gui/StatsWindow.h"
#include "window/gui/GuiWindow.h"
#include "window/gui/GuiMenuBar.h"
#include "libultraship/libultra/controller.h"

namespace LUS {

typedef struct {
    union {
        struct {
            void* Window;
            void* DeviceContext;
            void* Device;
        } Dx11;
        struct {
            void* Window;
            void* Context;
        } Opengl;
        struct {
            void* Window;
            SDL_Renderer* Renderer;
        } Metal;
        struct {
            uint32_t Width;
            uint32_t Height;
        } Gx2;
    };
} GuiWindowInitData;

typedef union {
    struct {
        void* Handle;
        int Msg;
        int Param1;
        int Param2;
    } Win32;
    struct {
        void* Event;
    } Sdl;
    struct {
        void* Input;
    } Gx2;
} WindowEvent;

class Gui {
  public:
    Gui();
    ~Gui();

    void Init(GuiWindowInitData windowImpl);
    void StartFrame();
    void EndFrame();
    void RenderViewports();
    void DrawMenu();

    void SaveConsoleVariablesOnNextTick();
    void Update(WindowEvent event);
    void AddGuiWindow(std::shared_ptr<GuiWindow> guiWindow);
    void RemoveGuiWindow(std::shared_ptr<GuiWindow> guiWindow);
    void RemoveGuiWindow(const std::string& name);
    void LoadGuiTexture(const std::string& name, const std::string& path, const ImVec4& tint);
    ImTextureID GetTextureByName(const std::string& name);
    bool SupportsViewports();
    std::shared_ptr<GuiWindow> GetGuiWindow(const std::string& name);
    std::shared_ptr<GameOverlay> GetGameOverlay();
    std::shared_ptr<InputViewer> GetInputViewer();
    void SetMenuBar(std::shared_ptr<GuiMenuBar> menuBar);
    std::shared_ptr<GuiMenuBar> GetMenuBar();
    void LoadTexture(const std::string& name, const std::string& path);

  protected:
    void ImGuiWMInit();
    void ImGuiBackendInit();
    void ImGuiBackendNewFrame();
    void ImGuiWMNewFrame();
    void ImGuiRenderDrawData(ImDrawData* data);
    ImTextureID GetTextureById(int32_t id);
    void ApplyResolutionChanges();

  private:
    struct GuiTexture {
        uint32_t RendererTextureId;
        int32_t Width;
        int32_t Height;
    };

    GuiWindowInitData mImpl;
    ImGuiIO* mImGuiIo;
    bool mNeedsConsoleVariableSave;
    std::shared_ptr<GameOverlay> mGameOverlay;
    std::shared_ptr<InputViewer> mInputViewer;
    std::shared_ptr<GuiMenuBar> mMenuBar;
    std::map<std::string, GuiTexture> mGuiTextures;
    std::map<std::string, std::shared_ptr<GuiWindow>> mGuiWindows;
};
} // namespace LUS

#endif
