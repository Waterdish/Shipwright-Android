#define NOMINMAX

#include "Gui.h"

#include <cstring>
#include <utility>
#include <string>
#include <vector>

#include "config/Config.h"
#include "Context.h"
#include "public/bridge/consolevariablebridge.h"
#include "resource/type/Texture.h"
#include "graphic/Fast3D/gfx_pc.h"
#include "resource/File.h"
#include <stb/stb_image.h>
#include "window/gui/Fonts.h"

#ifdef __WIIU__
#include <gx2/registers.h> // GX2SetViewport / GX2SetScissor

#include <ImGui/backends/wiiu/imgui_impl_gx2.h>
#include <ImGui/backends/wiiu/imgui_impl_wiiu.h>

#include "graphic/Fast3D/gfx_wiiu.h"
#include "graphic/Fast3D/gfx_gx2.h"
#endif

#ifdef __APPLE__
#include <SDL_hints.h>
#include <SDL_video.h>

#include "graphic/Fast3D/gfx_metal.h"
#include <ImGui/backends/imgui_impl_metal.h>
#include <ImGui/backends/imgui_impl_sdl2.h>
#else
#include <SDL2/SDL_hints.h>
#include <SDL2/SDL_video.h>
#endif

#ifdef __SWITCH__
#include "port/switch/SwitchImpl.h"
#endif

#ifdef ENABLE_OPENGL
#include <ImGui/backends/imgui_impl_opengl3.h>
#include <ImGui/backends/imgui_impl_sdl2.h>

#endif

#if defined(ENABLE_DX11) || defined(ENABLE_DX12)
#include <graphic/Fast3D/gfx_direct3d11.h>
#include <ImGui/backends/imgui_impl_dx11.h>
#include <ImGui/backends/imgui_impl_win32.h>

// NOLINTNEXTLINE
IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

#endif

namespace LUS {
#define TOGGLE_BTN ImGuiKey_F1
#define TOGGLE_PAD_BTN ImGuiKey_GamepadBack


Gui::Gui() : mNeedsConsoleVariableSave(false) {
    mGameOverlay = std::make_shared<GameOverlay>();
    mInputViewer = std::make_shared<InputViewer>();

    AddGuiWindow(std::make_shared<StatsWindow>("gStatsEnabled", "Stats"));
    AddGuiWindow(std::make_shared<InputEditorWindow>("gControllerConfigurationEnabled", "Input Editor"));
    AddGuiWindow(std::make_shared<ConsoleWindow>("gConsoleEnabled", "Console"));
}

Gui::~Gui() {
    SPDLOG_TRACE("destruct gui");
}

void Gui::Init(GuiWindowInitData windowImpl) {
    SDL_Log("GUI initing");
    mImpl = windowImpl;
    ImGuiContext* ctx = ImGui::CreateContext();
    ImGui::SetCurrentContext(ctx);
    mImGuiIo = &ImGui::GetIO();
    mImGuiIo->ConfigFlags |= ImGuiConfigFlags_DockingEnable | ImGuiConfigFlags_NoMouseCursorChange;


    // Add Font Awesome and merge it into the default font.
    mImGuiIo->Fonts->AddFontDefault();
    // This must match the default font size, which is 13.0f.
    float baseFontSize = 13.0f;
    // FontAwesome fonts need to have their sizes reduced by 2.0f/3.0f in order to align correctly
    float iconFontSize = baseFontSize * 2.0f / 3.0f;
    static const ImWchar sIconsRanges[] = { ICON_MIN_FA, ICON_MAX_16_FA, 0 };
    ImFontConfig iconsConfig;
    iconsConfig.MergeMode = true;
    iconsConfig.PixelSnapH = true;
    iconsConfig.GlyphMinAdvanceX = iconFontSize;
    mImGuiIo->Fonts->AddFontFromMemoryCompressedBase85TTF(fontawesome_compressed_data_base85, iconFontSize,
                                                          &iconsConfig, sIconsRanges);


#ifdef __SWITCH__
    LUS::Switch::ImGuiSetupFont(mImGuiIo->Fonts);
#endif

#ifdef __WIIU__
    // Scale everything by 2 for the Wii U
    ImGui::GetStyle().ScaleAllSizes(2.0f);
    mImGuiIo->FontGlobalScale = 2.0f;

    // Setup display sizes
    mImGuiIo->DisplaySize.x = mImpl.Gx2.Width;
    mImGuiIo->DisplaySize.y = mImpl.Gx2.Height;
#endif

    SDL_Log("getting config path");
    auto imguiIniPath = LUS::Context::GetPathRelativeToAppDirectory("imgui.ini");
    auto imguiLogPath = LUS::Context::GetPathRelativeToAppDirectory("imgui_log.txt");
    SDL_Log("got config path");
    mImGuiIo->IniFilename = strcpy(new char[imguiIniPath.length() + 1], imguiIniPath.c_str());
    mImGuiIo->LogFilename = strcpy(new char[imguiLogPath.length() + 1], imguiLogPath.c_str());
    SDL_Log("copied file");

    SDL_Log("supports viewports");
    if (SupportsViewports() && CVarGetInteger("gEnableMultiViewports", 1)) {
        mImGuiIo->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    }

    //GetMenuBar()->SetVisiblity(true);
    SDL_Log("controlnav");
    if (CVarGetInteger("gControlNav", 0)) {
        mImGuiIo->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    } else {
        mImGuiIo->ConfigFlags &= ~ImGuiConfigFlags_NavEnableGamepad;
    }

    SDL_Log("init gui window");
    GetGuiWindow("Stats")->Init();
    GetGuiWindow("Input Editor")->Init();
    GetGuiWindow("Console")->Init();
    GetGameOverlay()->Init();
    SDL_Log("gui window initd");

    SDL_Log("imgui wm init");
    ImGuiWMInit();
    SDL_Log("initd imgui wm");
    ImGuiBackendInit();
    SDL_Log("imgui background and wm initd");
#ifdef __SWITCH__
    ImGui::GetStyle().ScaleAllSizes(2); //copied below
#endif

    ImGui::GetStyle().ScaleAllSizes(2);
    CVarClear("gNewFileDropped");
    CVarClear("gDroppedFile");

#ifdef __SWITCH__
    Switch::ApplyOverclock();
#endif
}

void Gui::ImGuiWMInit() {
    SDL_Log("try to get window backend.");

    switch (Context::GetInstance()->GetWindow()->GetWindowBackend()) {
#ifdef __WIIU__
        case WindowBackend::GX2:
            ImGui_ImplWiiU_Init();
            break;
#else
        case WindowBackend::SDL_OPENGL:
            SDL_Log("correct wm init");
            SDL_SetHint(SDL_HINT_TOUCH_MOUSE_EVENTS, "1");
            SDL_SetHint(SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS, "1");
            ImGui_ImplSDL2_InitForOpenGL(static_cast<SDL_Window*>(mImpl.Opengl.Window), mImpl.Opengl.Context);
            break;
#endif
#if __APPLE__
        case WindowBackend::SDL_METAL:
            SDL_SetHint(SDL_HINT_TOUCH_MOUSE_EVENTS, "1");
            SDL_SetHint(SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS, "1");
            ImGui_ImplSDL2_InitForMetal(static_cast<SDL_Window*>(mImpl.Metal.Window));
            break;
#endif
#if defined(ENABLE_DX11) || defined(ENABLE_DX12)
        case WindowBackend::DX11:
            ImGui_ImplWin32_Init(mImpl.Dx11.Window);
            break;
#endif
        default:
            break;
    }
}

void Gui::ImGuiBackendInit() {
    switch (Context::GetInstance()->GetWindow()->GetWindowBackend()) {
#ifdef __WIIU__
        case WindowBackend::GX2:
            ImGui_ImplGX2_Init();
            break;
#else
        case WindowBackend::SDL_OPENGL:
#ifdef __APPLE__
            ImGui_ImplOpenGL3_Init("#version 410 core");
#else
            //modified for android
            ImGui_ImplOpenGL3_Init("#version 300 es"); //init here
#endif
            break;
#endif

#ifdef __APPLE__
        case WindowBackend::SDL_METAL:
            Metal_Init(mImpl.Metal.Renderer);
            break;
#endif

#if defined(ENABLE_DX11) || defined(ENABLE_DX12)
        case WindowBackend::DX11:
            ImGui_ImplDX11_Init(static_cast<ID3D11Device*>(mImpl.Dx11.Device),
                                static_cast<ID3D11DeviceContext*>(mImpl.Dx11.DeviceContext));
            break;
#endif
        default:
            break;
    }
}

void Gui::LoadTexture(const std::string& name, const std::string& path) {
    // TODO: Nothing ever unloads the texture from Fast3D here.
    GfxRenderingAPI* api = gfx_get_current_rendering_api();
    const auto res = Context::GetInstance()->GetResourceManager()->LoadFile(path);

    GuiTexture asset;
    asset.RendererTextureId = api->new_texture();
    asset.Width = 0;
    asset.Height = 0;
    uint8_t* imgData = stbi_load_from_memory(reinterpret_cast<const stbi_uc*>(res->Buffer.data()), res->Buffer.size(),
                                             &asset.Width, &asset.Height, nullptr, 4);

    if (imgData == nullptr) {
        SPDLOG_ERROR("Error loading imgui texture {}", stbi_failure_reason());
        return;
    }

    api->select_texture(0, asset.RendererTextureId);
    api->set_sampler_parameters(0, false, 0, 0);
    api->upload_texture(imgData, asset.Width, asset.Height);

    mGuiTextures[name] = asset;
    stbi_image_free(imgData);
}

bool Gui::SupportsViewports() {
#ifdef __SWITCH__
    return false;
#endif

    switch (Context::GetInstance()->GetWindow()->GetWindowBackend()) {
        case WindowBackend::DX11:
            return true;
        case WindowBackend::SDL_OPENGL:
        case WindowBackend::SDL_METAL:
            return true;
        default:
            return false;
    }
}

void Gui::Update(WindowEvent event) {
    if (mNeedsConsoleVariableSave) {
        CVarSave();
        mNeedsConsoleVariableSave = false;
    }

    switch (Context::GetInstance()->GetWindow()->GetWindowBackend()) {
#ifdef __WIIU__
        case WindowBackend::GX2:
            if (!ImGui_ImplWiiU_ProcessInput((ImGui_ImplWiiU_ControllerInput*)event.Gx2.Input)) {}
            break;
#else
        case WindowBackend::SDL_OPENGL:
        case WindowBackend::SDL_METAL:
            ImGui_ImplSDL2_ProcessEvent(static_cast<const SDL_Event*>(event.Sdl.Event));

#ifdef __SWITCH__
            LUS::Switch::ImGuiProcessEvent(mImGuiIo->WantTextInput);
#endif
            break;
#endif
#if defined(ENABLE_DX11) || defined(ENABLE_DX12)
        case WindowBackend::DX11:
            ImGui_ImplWin32_WndProcHandler(static_cast<HWND>(event.Win32.Handle), event.Win32.Msg, event.Win32.Param1,
                                           event.Win32.Param2);
            break;
#endif
        default:
            break;
    }
}

void Gui::DrawMenu() {
    LUS::Context::GetInstance()->GetWindow()->GetGui()->GetGuiWindow("Console")->Update();
    ImGuiBackendNewFrame();
    ImGuiWMNewFrame();
    ImGui::NewFrame();

    const std::shared_ptr<Window> wnd = Context::GetInstance()->GetWindow();
    const std::shared_ptr<Config> conf = Context::GetInstance()->GetConfig();

    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoBackground |
                                   ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove |
                                   ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus |
                                   ImGuiWindowFlags_NoResize;

    if (GetMenuBar() && GetMenuBar()->IsVisible()) {
        windowFlags |= ImGuiWindowFlags_MenuBar;
    }

    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(ImVec2((int)wnd->GetWidth(), (int)wnd->GetHeight()));
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 0.0f);
    ImGui::Begin("Main - Deck", nullptr, windowFlags);
    ImGui::PopStyleVar(3);

    ImVec2 topLeftPos = ImGui::GetWindowPos();

    const ImGuiID dockId = ImGui::GetID("main_dock");

    if (!ImGui::DockBuilderGetNode(dockId)) {
        ImGui::DockBuilderRemoveNode(dockId);
        ImGui::DockBuilderAddNode(dockId, ImGuiDockNodeFlags_NoTabBar);

        ImGui::DockBuilderDockWindow("Main Game", dockId);

        ImGui::DockBuilderFinish(dockId);
    }

    ImGui::DockSpace(dockId, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None | ImGuiDockNodeFlags_NoDockingInCentralNode);
    if (ImGui::IsKeyPressed(TOGGLE_BTN) || (ImGui::IsKeyPressed(TOGGLE_PAD_BTN) && CVarGetInteger("gControlNav", 0))) {
        GetMenuBar()->ToggleVisibility();
        if (wnd->IsFullscreen()) {
            Context::GetInstance()->GetWindow()->SetCursorVisibility(GetMenuBar() && GetMenuBar()->IsVisible());
        }
        Context::GetInstance()->GetControlDeck()->SaveSettings();
        if (CVarGetInteger("gControlNav", 0) && GetMenuBar() && GetMenuBar()->IsVisible()) {
            mImGuiIo->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
        } else {
            mImGuiIo->ConfigFlags &= ~ImGuiConfigFlags_NavEnableGamepad;
        }
    }

#if __APPLE__
    if ((ImGui::IsKeyDown(ImGuiKey_LeftSuper) || ImGui::IsKeyDown(ImGuiKey_RightSuper)) &&
        ImGui::IsKeyPressed(ImGuiKey_R, false)) {
        std::reinterpret_pointer_cast<LUS::ConsoleWindow>(
            LUS::Context::GetInstance()->GetWindow()->GetGui()->GetGuiWindow("Console"))
            ->Dispatch("reset");
    }
#else
    if ((ImGui::IsKeyDown(ImGuiKey_LeftCtrl) || ImGui::IsKeyDown(ImGuiKey_RightCtrl)) &&
        ImGui::IsKeyPressed(ImGuiKey_R, false)) {
        std::reinterpret_pointer_cast<LUS::ConsoleWindow>(
            LUS::Context::GetInstance()->GetWindow()->GetGui()->GetGuiWindow("Console"))
            ->Dispatch("reset");
    }
#endif

    if (GetMenuBar()) {
        GetMenuBar()->Draw();
    }

    ImGui::End();

    for (auto& windowIter : mGuiWindows) {
        windowIter.second->Update();
        windowIter.second->Draw();
    }

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 0.0f);
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
                             ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground;
    ImGui::Begin("Main Game", nullptr, flags);
    ImGui::PopStyleVar(3);
    ImGui::PopStyleColor();

    ImVec2 mainPos = ImGui::GetWindowPos();
    mainPos.x -= topLeftPos.x;
    mainPos.y -= topLeftPos.y;
    ImVec2 size = ImGui::GetContentRegionAvail();
    gfx_current_dimensions.width = (uint32_t)(size.x * gfx_current_dimensions.internal_mul);
    gfx_current_dimensions.height = (uint32_t)(size.y * gfx_current_dimensions.internal_mul);
    gfx_current_game_window_viewport.x = (int16_t)mainPos.x;
    gfx_current_game_window_viewport.y = (int16_t)mainPos.y;
    gfx_current_game_window_viewport.width = (int16_t)size.x;
    gfx_current_game_window_viewport.height = (int16_t)size.y;

    if (CVarGetInteger("gAdvancedResolution.Enabled", 0)) {
        ApplyResolutionChanges();
    }

    switch (CVarGetInteger("gLowResMode", 0)) {
        case 1: { // N64 Mode
            gfx_current_dimensions.width = 320;
            gfx_current_dimensions.height = 240;
            /*
            const int sw = size.y * 320 / 240;
            gfx_current_game_window_viewport.x += ((int)size.x - sw) / 2;
            gfx_current_game_window_viewport.width = sw;*/
            break;
        }
        case 2: { // 240p Widescreen
            const int vertRes = 240;
            gfx_current_dimensions.width = vertRes * size.x / size.y;
            gfx_current_dimensions.height = vertRes;
            break;
        }
        case 3: { // 480p Widescreen
            const int vertRes = 480;
            gfx_current_dimensions.width = vertRes * size.x / size.y;
            gfx_current_dimensions.height = vertRes;
            break;
        }
    }

    GetGameOverlay()->Draw();
    GetInputViewer()->Draw();
}

void Gui::ImGuiBackendNewFrame() {
    switch (Context::GetInstance()->GetWindow()->GetWindowBackend()) {
#ifdef __WIIU__
        case WindowBackend::GX2:
            mImGuiIo->DeltaTime = (float)frametime / 1000.0f / 1000.0f;
            ImGui_ImplGX2_NewFrame();
            break;
#else
        case WindowBackend::SDL_OPENGL:
            ImGui_ImplOpenGL3_NewFrame();
            break;
#endif
#ifdef __APPLE__
        case WindowBackend::SDL_METAL:
            Metal_NewFrame(mImpl.Metal.Renderer);
            break;
#endif
#if defined(ENABLE_DX11) || defined(ENABLE_DX12)
        case WindowBackend::DX11:
            ImGui_ImplDX11_NewFrame();
            break;
#endif
        default:
            break;
    }
}

void Gui::ImGuiWMNewFrame() {
    switch (Context::GetInstance()->GetWindow()->GetWindowBackend()) {
#ifdef __WIIU__
        case WindowBackend::GX2:
            break;
#else
        case WindowBackend::SDL_OPENGL:
        case WindowBackend::SDL_METAL:
            ImGui_ImplSDL2_NewFrame();
            break;
#endif
#if defined(ENABLE_DX11) || defined(ENABLE_DX12)
        case WindowBackend::DX11:
            ImGui_ImplWin32_NewFrame();
            break;
#endif
        default:
            break;
    }
}

void Gui::ApplyResolutionChanges() {
    ImVec2 size = ImGui::GetContentRegionAvail();

    const float aspectRatioX = CVarGetFloat("gAdvancedResolution.AspectRatioX", 16.0f);
    const float aspectRatioY = CVarGetFloat("gAdvancedResolution.AspectRatioY", 9.0f);
    const uint32_t verticalPixelCount = CVarGetInteger("gAdvancedResolution.VerticalPixelCount", 480);
    const bool verticalResolutionToggle = CVarGetInteger("gAdvancedResolution.VerticalResolutionToggle", 0);

    const bool aspectRatioIsEnabled = (aspectRatioX > 0.0f) && (aspectRatioY > 0.0f);

    const uint32_t minResolutionWidth = 320;
    const uint32_t minResolutionHeight = 240;
    const uint32_t maxResolutionWidth = 8096;  // the renderer's actual limit is 16384
    const uint32_t maxResolutionHeight = 4320; // on either axis. if you have the VRAM for it.
    uint32_t newWidth = gfx_current_dimensions.width;
    uint32_t newHeight = gfx_current_dimensions.height;

    if (verticalResolutionToggle) { // Use fixed vertical resolution
        if (aspectRatioIsEnabled) {
            newWidth = uint32_t(float(verticalPixelCount / aspectRatioY) * aspectRatioX);
        } else {
            newWidth = uint32_t(float(verticalPixelCount * size.x / size.y));
        }
        newHeight = verticalPixelCount;
    } else { // Use the window's resolution
        if (aspectRatioIsEnabled) {
            if (((float)gfx_current_game_window_viewport.width / gfx_current_game_window_viewport.height) >
                (aspectRatioX / aspectRatioY)) {
                // when pillarboxed
                newWidth = uint32_t(float(gfx_current_dimensions.height / aspectRatioY) * aspectRatioX);
            } else { // when letterboxed
                newHeight = uint32_t(float(gfx_current_dimensions.width / aspectRatioX) * aspectRatioY);
            }
        } // else, having both options turned off does nothing.
    }
    // clamp values to prevent renderer crash
    if (newWidth < minResolutionWidth) {
        newWidth = minResolutionWidth;
    }
    if (newHeight < minResolutionHeight) {
        newHeight = minResolutionHeight;
    }
    if (newWidth > maxResolutionWidth) {
        newWidth = maxResolutionWidth;
    }
    if (newHeight > maxResolutionHeight) {
        newHeight = maxResolutionHeight;
    }
    // apply new dimensions
    gfx_current_dimensions.width = newWidth;
    gfx_current_dimensions.height = newHeight;
    // centring the image is done in Gui::StartFrame().
}

void Gui::StartFrame() {
    const ImVec2 mainPos = ImGui::GetWindowPos();
    ImVec2 size = ImGui::GetContentRegionAvail();
    ImVec2 pos = ImVec2(0, 0);
    if (CVarGetInteger("gLowResMode", 0) == 1) { // N64 Mode takes priority
        const float sw = size.y * 320.0f / 240.0f;
        pos = ImVec2(size.x / 2 - sw / 2, 0);
        size = ImVec2(sw, size.y);
    } else if (CVarGetInteger("gAdvancedResolution.Enabled", 0)) {
        if (!CVarGetInteger("gAdvancedResolution.PixelPerfectMode", 0)) {
            if (!CVarGetInteger("gAdvancedResolution.IgnoreAspectCorrection", 0)) {
                float sWdth = size.y * gfx_current_dimensions.width / gfx_current_dimensions.height;
                float sHght = size.x * gfx_current_dimensions.height / gfx_current_dimensions.width;
                float sPosX = size.x / 2 - sWdth / 2;
                float sPosY = size.y / 2 - sHght / 2;
                if (sPosY < 0.0f) { // pillarbox
                    sPosY = 0.0f;   // clamp y position
                    sHght = size.y; // reset height
                }
                if (sPosX < 0.0f) { // letterbox
                    sPosX = 0.0f;   // clamp x position
                    sWdth = size.x; // reset width
                }
                pos = ImVec2(sPosX, sPosY);
                size = ImVec2(sWdth, sHght);
            }
        } else { // in pixel perfect mode it's much easier
            const int factor = CVarGetInteger("gAdvancedResolution.IntegerScaleFactor", 1);
            float sPosX = size.x / 2 - (gfx_current_dimensions.width * factor) / 2;
            float sPosY = size.y / 2 - (gfx_current_dimensions.height * factor) / 2;
            pos = ImVec2(sPosX, sPosY);
            size = ImVec2(float(gfx_current_dimensions.width) * factor, float(gfx_current_dimensions.height) * factor);
        }
    }
    if (gfxFramebuffer) {
        ImGui::SetCursorPos(pos);
        ImGui::Image(reinterpret_cast<ImTextureID>(gfxFramebuffer), size);
    }

    ImGui::End();
}

void Gui::RenderViewports() {
    ImGui::Render();
    ImGuiRenderDrawData(ImGui::GetDrawData());
    if (mImGuiIo->ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        WindowBackend backend = Context::GetInstance()->GetWindow()->GetWindowBackend();
        if ((backend == WindowBackend::SDL_OPENGL || backend == WindowBackend::SDL_METAL) &&
            mImpl.Opengl.Context != nullptr) {
            SDL_Window* backupCurrentWindow = SDL_GL_GetCurrentWindow();
            SDL_GLContext backupCurrentContext = SDL_GL_GetCurrentContext();

            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();

            SDL_GL_MakeCurrent(backupCurrentWindow, backupCurrentContext);
        } else {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }
    }
}

ImTextureID Gui::GetTextureById(int32_t id) {
#ifdef ENABLE_DX11
    if (Context::GetInstance()->GetWindow()->GetWindowBackend() == WindowBackend::DX11) {
        return gfx_d3d11_get_texture_by_id(id);
    }
#endif
#ifdef __APPLE__
    if (Context::GetInstance()->GetWindow()->GetWindowBackend() == WindowBackend::SDL_METAL) {
        return gfx_metal_get_texture_by_id(id);
    }
#endif
#ifdef __WIIU__
    if (Context::GetInstance()->GetWindow()->GetWindowBackend() == WindowBackend::GX2) {
        return gfx_gx2_texture_for_imgui(id);
    }
#endif

    return reinterpret_cast<ImTextureID>(id);
}

ImTextureID Gui::GetTextureByName(const std::string& name) {
    return GetTextureById(mGuiTextures[name].RendererTextureId);
}

void Gui::ImGuiRenderDrawData(ImDrawData* data) {
    switch (Context::GetInstance()->GetWindow()->GetWindowBackend()) {
#ifdef __WIIU__
        case WindowBackend::GX2:
            ImGui_ImplGX2_RenderDrawData(data);

            // Reset viewport and scissor for drawing the keyboard
            GX2SetViewport(0.0f, 0.0f, mImGuiIo->DisplaySize.x, mImGuiIo->DisplaySize.y, 0.0f, 1.0f);
            GX2SetScissor(0, 0, mImGuiIo->DisplaySize.x, mImGuiIo->DisplaySize.y);
            ImGui_ImplWiiU_DrawKeyboardOverlay();
            break;
#else
        case WindowBackend::SDL_OPENGL:
            //SDL_Log("commented out so I know where it fails");
            ImGui_ImplOpenGL3_RenderDrawData(data);
            break;
#endif
#ifdef __APPLE__
        case WindowBackend::SDL_METAL:
            Metal_RenderDrawData(data);
            break;
#endif
#if defined(ENABLE_DX11) || defined(ENABLE_DX12)
        case WindowBackend::DX11:
            ImGui_ImplDX11_RenderDrawData(data);
            break;
#endif
        default:
            break;
    }
}

void Gui::EndFrame() {
    ImGui::EndFrame();
    if (mImGuiIo->ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        ImGui::UpdatePlatformWindows();
    }
}

void Gui::SaveConsoleVariablesOnNextTick() {
    mNeedsConsoleVariableSave = true;
}

void Gui::AddGuiWindow(std::shared_ptr<GuiWindow> guiWindow) {
    if (mGuiWindows.contains(guiWindow->GetName())) {
        SPDLOG_ERROR("ImGui::AddGuiWindow: Attempting to add duplicate window name {}", guiWindow->GetName());
        return;
    }

    mGuiWindows[guiWindow->GetName()] = guiWindow;
    guiWindow->Init();
}

void Gui::RemoveGuiWindow(std::shared_ptr<GuiWindow> guiWindow) {
    RemoveGuiWindow(guiWindow->GetName());
}

void Gui::RemoveGuiWindow(const std::string& name) {
    mGuiWindows.erase(name);
}

std::shared_ptr<GuiWindow> Gui::GetGuiWindow(const std::string& name) {
    if (mGuiWindows.contains(name)) {
        return mGuiWindows[name];
    } else {
        return nullptr;
    }
}

void Gui::LoadGuiTexture(const std::string& name, const std::string& path, const ImVec4& tint) {
    GfxRenderingAPI* api = gfx_get_current_rendering_api();
    const auto res =
        static_cast<LUS::Texture*>(Context::GetInstance()->GetResourceManager()->LoadResource(path, true).get());

    std::vector<uint8_t> texBuffer;
    texBuffer.reserve(res->Width * res->Height * 4);

    // For HD textures we need to load the buffer raw (similar to inside gfx_pp)
    if ((res->Flags & TEX_FLAG_LOAD_AS_RAW) != 0) {
        // Raw loading doesn't support TLUT textures
        if (res->Type == LUS::TextureType::Palette4bpp || res->Type == LUS::TextureType::Palette8bpp) {
            // TODO convert other image types
            SPDLOG_WARN("ImGui::ResourceLoad: Attempting to load unsupported image type %s", path.c_str());
            return;
        }

        texBuffer.assign(res->ImageData, res->ImageData + (res->Width * res->Height * 4));
    } else {
        switch (res->Type) {
            case LUS::TextureType::RGBA32bpp:
                texBuffer.assign(res->ImageData, res->ImageData + (res->Width * res->Height * 4));
                break;
            case LUS::TextureType::GrayscaleAlpha8bpp:
                for (int32_t i = 0; i < res->Width * res->Height; i++) {
                    uint8_t ia = res->ImageData[i];
                    uint8_t color = ((ia >> 4) & 0xF) * 255 / 15;
                    uint8_t alpha = (ia & 0xF) * 255 / 15;
                    texBuffer.push_back(color);
                    texBuffer.push_back(color);
                    texBuffer.push_back(color);
                    texBuffer.push_back(alpha);
                }
                break;
            default:
                // TODO convert other image types
                SPDLOG_WARN("ImGui::ResourceLoad: Attempting to load unsupported image type %s", path.c_str());
                return;
        }
    }

    for (size_t pixel = 0; pixel < texBuffer.size() / 4; pixel++) {
        texBuffer[pixel * 4 + 0] *= tint.x;
        texBuffer[pixel * 4 + 1] *= tint.y;
        texBuffer[pixel * 4 + 2] *= tint.z;
        texBuffer[pixel * 4 + 3] *= tint.w;
    }

    GuiTexture asset;
    asset.RendererTextureId = api->new_texture();
    asset.Width = res->Width;
    asset.Height = res->Height;

    api->select_texture(0, asset.RendererTextureId);
    api->set_sampler_parameters(0, false, 0, 0);
    api->upload_texture(texBuffer.data(), res->Width, res->Height);

    mGuiTextures[name] = asset;
}

std::shared_ptr<GameOverlay> Gui::GetGameOverlay() {
    return mGameOverlay;
}

std::shared_ptr<InputViewer> Gui::GetInputViewer() {
    return mInputViewer;
}

void Gui::SetMenuBar(std::shared_ptr<GuiMenuBar> menuBar) {
    mMenuBar = menuBar;

    if (GetMenuBar()) {
        GetMenuBar()->Init();
    }

    if (Context::GetInstance()->GetWindow()->IsFullscreen()) {
        Context::GetInstance()->GetWindow()->SetCursorVisibility(GetMenuBar() && GetMenuBar()->IsVisible());
    }
}

std::shared_ptr<GuiMenuBar> Gui::GetMenuBar() {
    return mMenuBar;
}
} // namespace LUS
