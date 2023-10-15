#include "ConsoleWindow.h"

#include "public/bridge/consolevariablebridge.h"
#include "Context.h"
#include <Utils/StringHelper.h>
#include "utils/Utils.h"
#include <sstream>

namespace LUS {

int32_t ConsoleWindow::HelpCommand(std::shared_ptr<Console> console, const std::vector<std::string>& args,
                                   std::string* output) {
    if (output) {
        *output += "Commands:\n";
        for (const auto& cmd : console->GetCommands()) {
            *output += " - " + cmd.first + "\n";
        }

        return 0;
    }

    return 1;
}

int32_t ConsoleWindow::ClearCommand(std::shared_ptr<Console> console, const std::vector<std::string>& args,
                                    std::string* output) {
    auto window = std::static_pointer_cast<LUS::ConsoleWindow>(
        Context::GetInstance()->GetWindow()->GetGui()->GetGuiWindow("Console"));
    if (!window) {
        if (output) {
            *output += "A console window is necessary for Clear";
        }

        return 1;
    }

    window->ClearLogs(window->GetCurrentChannel());
    return 0;
}

int32_t ConsoleWindow::BindCommand(std::shared_ptr<Console> console, const std::vector<std::string>& args,
                                   std::string* output) {
    if (args.size() > 2) {
        auto window = std::static_pointer_cast<LUS::ConsoleWindow>(
            Context::GetInstance()->GetWindow()->GetGui()->GetGuiWindow("Console"));
        if (!window) {
            if (output) {
                *output += "A console window is necessary for Bind";
            }

            return 1;
        }

        const ImGuiIO* io = &ImGui::GetIO();

        for (size_t k = 0; k < std::size(io->KeysData); k++) {
            std::string key(ImGui::GetKeyName((ImGuiKey)k));

            if (toLowerCase(args[1]) == toLowerCase(key)) {
                std::vector<std::string> tmp;
                const char* const delim = " ";
                std::ostringstream imploded;
                std::copy(args.begin() + 2, args.end(), std::ostream_iterator<std::string>(imploded, delim));
                window->mBindings[(ImGuiKey)k] = imploded.str();
                if (output) {
                    *output += "Binding '" + args[1] + " to " + window->mBindings[(ImGuiKey)k];
                }
                break;
            }
        }
    } else {
        if (output) {
            *output += "Not enough arguments";
        }
        return 1;
    }

    return 0;
}

int32_t ConsoleWindow::BindToggleCommand(std::shared_ptr<Console> console, const std::vector<std::string>& args,
                                         std::string* output) {
    if (args.size() > 2) {
        auto window = std::static_pointer_cast<LUS::ConsoleWindow>(
            Context::GetInstance()->GetWindow()->GetGui()->GetGuiWindow("Console"));
        if (!window) {
            if (output) {
                *output += "A console window is necessary for BindToggle";
            }

            return 1;
        }

        const ImGuiIO* io = &ImGui::GetIO();

        for (size_t k = 0; k < std::size(io->KeysData); k++) {
            std::string key(ImGui::GetKeyName((ImGuiKey)k));

            if (toLowerCase(args[1]) == toLowerCase(key)) {
                window->mBindingToggle[(ImGuiKey)k] = args[2];
                window->SendInfoMessage("Binding toggle '%s' to %s", args[1].c_str(),
                                        window->mBindingToggle[(ImGuiKey)k].c_str());
                break;
            }
        }
    } else {
        if (output) {
            *output += "Missing arguments";
        }
        return 1;
    }

    return 0;
}

#define VARTYPE_INTEGER 0
#define VARTYPE_FLOAT 1
#define VARTYPE_STRING 2
#define VARTYPE_RGBA 3

int32_t ConsoleWindow::SetCommand(std::shared_ptr<Console> console, const std::vector<std::string>& args,
                                  std::string* output) {
    if (args.size() < 3) {
        if (output) {
            *output += "Not enough arguments.";
        }

        return 1;
    }

    int vType = CheckVarType(args[2]);

    if (vType == VARTYPE_STRING) {
        CVarSetString(args[1].c_str(), args[2].c_str());
    } else if (vType == VARTYPE_FLOAT) {
        CVarSetFloat((char*)args[1].c_str(), std::stof(args[2]));
    } else if (vType == VARTYPE_RGBA) {
        uint32_t val = std::stoul(&args[2].c_str()[1], nullptr, 16);
        Color_RGBA8 clr;
        clr.r = val >> 24;
        clr.g = val >> 16;
        clr.b = val >> 8;
        clr.a = val & 0xFF;
        CVarSetColor((char*)args[1].c_str(), clr);
    } else {
        CVarSetInteger(args[1].c_str(), std::stoi(args[2]));
    }

    CVarSave();

    return 0;
}

int32_t ConsoleWindow::GetCommand(std::shared_ptr<Console> console, const std::vector<std::string>& args,
                                  std::string* output) {
    if (args.size() < 2) {
        if (output) {
            *output += "Not enough arguments.";
        }

        return 1;
    }

    auto cvar = CVarGet(args[1].c_str());

    if (cvar != nullptr) {
        if (cvar->Type == LUS::ConsoleVariableType::Integer) {
            if (output) {
                *output += StringHelper::Sprintf("[LUS] Variable %s is %i", args[1].c_str(), cvar->Integer);
            }
        } else if (cvar->Type == LUS::ConsoleVariableType::Float) {
            if (output) {
                *output += StringHelper::Sprintf("[LUS] Variable %s is %f", args[1].c_str(), cvar->Float);
            }
        } else if (cvar->Type == LUS::ConsoleVariableType::String) {
            if (output) {
                *output += StringHelper::Sprintf("[LUS] Variable %s is %s", args[1].c_str(), cvar->String.c_str());
            }
        } else if (cvar->Type == LUS::ConsoleVariableType::Color) {
            if (output) {
                *output += StringHelper::Sprintf("[LUS] Variable %s is %08X", args[1].c_str(), cvar->Color);
            }
        } else {
            if (output) {
                *output += StringHelper::Sprintf("[LUS] Loaded CVar with unsupported type: %s", cvar->Type);
            }
            return 1;
        }
    } else {
        if (output) {
            *output += StringHelper::Sprintf("[LUS] Could not find variable %s", args[1].c_str());
        }
    }

    return 0;
}

int32_t ConsoleWindow::CheckVarType(const std::string& input) {
    int32_t result = VARTYPE_STRING;

    if (input[0] == '#') {
        return VARTYPE_RGBA;
    }

    for (size_t i = 0; i < input.size(); i++) {
        if (!(std::isdigit(input[i]) || input[i] == '.')) {
            break;
        } else {
            if (input[i] == '.') {
                result = VARTYPE_FLOAT;
            } else if (std::isdigit(input[i]) && result != VARTYPE_FLOAT) {
                result = VARTYPE_INTEGER;
            }
        }
    }

    return result;
}

ConsoleWindow::~ConsoleWindow() {
    SPDLOG_TRACE("destruct console window");
}

void ConsoleWindow::InitElement() {
    // TODO: These buffers are never freed.
    mInputBuffer = new char[gMaxBufferSize];
    strcpy(mInputBuffer, "");
    mFilterBuffer = new char[gMaxBufferSize];
    strcpy(mFilterBuffer, "");

    Context::GetInstance()->GetConsole()->AddCommand(
        "set", { SetCommand,
                 "Sets a console variable.",
                 { { "varName", LUS::ArgumentType::TEXT }, { "varValue", LUS::ArgumentType::TEXT } } });
    Context::GetInstance()->GetConsole()->AddCommand(
        "get", { GetCommand, "Bind key as a bool toggle", { { "varName", LUS::ArgumentType::TEXT } } });
    Context::GetInstance()->GetConsole()->AddCommand("help", { HelpCommand, "Shows all the commands" });
    Context::GetInstance()->GetConsole()->AddCommand("clear", { ClearCommand, "Clear the console history" });
    Context::GetInstance()->GetConsole()->AddCommand(
        "bind", { BindCommand,
                  "Binds key to commands",
                  { { "key", LUS::ArgumentType::TEXT }, { "cmd", LUS::ArgumentType::TEXT } } });
    Context::GetInstance()->GetConsole()->AddCommand(
        "bind-toggle", { BindToggleCommand,
                         "Bind key as a bool toggle",
                         { { "key", LUS::ArgumentType::TEXT }, { "cmd", LUS::ArgumentType::TEXT } } });
}

void ConsoleWindow::UpdateElement() {
    for (auto [key, cmd] : mBindings) {
        if (ImGui::IsKeyPressed(key)) {
            Dispatch(cmd);
        }
    }
    for (auto [key, var] : mBindingToggle) {
        if (ImGui::IsKeyPressed(key)) {
            Dispatch("set " + var + " " + std::to_string(!static_cast<bool>(CVarGetInteger(var.c_str(), 0))));
        }
    }
}

void ConsoleWindow::DrawElement() {
    bool inputFocus = false;

    ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
    ImGui::Begin("Console", &mIsVisible, ImGuiWindowFlags_NoFocusOnAppearing);
    const ImVec2 pos = ImGui::GetWindowPos();
    const ImVec2 size = ImGui::GetWindowSize();

    // Renders autocomplete window
    if (mOpenAutocomplete) {
        auto console = Context::GetInstance()->GetConsole();

        ImGui::SetNextWindowSize(ImVec2(350, std::min(static_cast<int>(mAutoComplete.size()), 3) * 20.f),
                                 ImGuiCond_Once);
        ImGui::SetNextWindowPos(ImVec2(pos.x + 8, pos.y + size.y - 1));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(3, 3));
        ImGui::Begin("##WndAutocomplete", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove);
        ImGui::BeginChild("AC_Child", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);
        ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(.3f, .3f, .3f, 1.0f));
        if (ImGui::BeginTable("AC_History", 1)) {
            for (const auto& cmd : mAutoComplete) {
                std::string usage = console->BuildUsage(cmd);
                std::string preview = cmd + " - " + console->GetCommand(cmd).Description;
                std::string autoComplete = (usage == "None" ? cmd : usage);
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                if (ImGui::Selectable(preview.c_str())) {
                    memset(mInputBuffer, 0, gMaxBufferSize);
                    memcpy(mInputBuffer, autoComplete.c_str(), sizeof(char) * autoComplete.size());
                    mOpenAutocomplete = false;
                    inputFocus = true;
                }
            }
            ImGui::EndTable();
        }
        if (ImGui::IsKeyPressed(ImGuiKey_Escape)) {
            mOpenAutocomplete = false;
        }
        ImGui::PopStyleColor();
        ImGui::EndChild();
        ImGui::End();
        ImGui::PopStyleVar();
    }

    if (ImGui::BeginPopupContextWindow("Context Menu")) {
        if (ImGui::MenuItem("Copy Text")) {
            ImGui::SetClipboardText(mLog[mCurrentChannel][mSelectedId].Text.c_str());
            mSelectedId = -1;
        }
        ImGui::EndPopup();
    }
    if (mSelectedId != -1 && ImGui::IsMouseClicked(1)) {
        ImGui::OpenPopup("##WndAutocomplete");
    }

    // Renders top bar filters
    if (ImGui::Button("Clear")) {
        mLog[mCurrentChannel].clear();
    }

    if (CVarGetInteger("gSinkEnabled", 0)) {
        ImGui::SameLine();
        ImGui::SetNextItemWidth(150);
        if (ImGui::BeginCombo("##channel", mCurrentChannel.c_str())) {
            for (const auto& channel : mLogChannels) {
                const bool isSelected = (channel == std::string(mCurrentChannel));
                if (ImGui::Selectable(channel.c_str(), isSelected)) {
                    mCurrentChannel = channel;
                }
                if (isSelected) {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }
    } else {
        mCurrentChannel = "Console";
    }
    ImGui::SameLine();
    ImGui::SetNextItemWidth(150);

    if (mCurrentChannel != "Console") {
        if (ImGui::BeginCombo("##level", spdlog::level::to_string_view(mLevelFilter).data())) {
            for (const auto& priorityFilter : mPriorityFilters) {
                const bool isSelected = priorityFilter == mLevelFilter;
                if (ImGui::Selectable(spdlog::level::to_string_view(priorityFilter).data(), isSelected)) {
                    mLevelFilter = priorityFilter;
                    if (isSelected) {
                        ImGui::SetItemDefaultFocus();
                    }
                }
            }
            ImGui::EndCombo();
        }
    } else {
        mLevelFilter = spdlog::level::trace;
    }
    ImGui::SameLine();
    ImGui::PushItemWidth(-1);
    if (ImGui::InputTextWithHint("##input", "Filter", mFilterBuffer, gMaxBufferSize)) {
        mFilter = std::string(mFilterBuffer);
    }
    ImGui::PopItemWidth();

    // Renders console history
    const float footerHeightToReserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
    ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footerHeightToReserve), false,
                      ImGuiWindowFlags_HorizontalScrollbar);
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(.3f, .3f, .3f, 1.0f));
    if (ImGui::BeginTable("History", 1)) {

        if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_DownArrow))) {
            if (mSelectedId < (int32_t)mLog.size() - 1) {
                ++mSelectedId;
            }
        }
        if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_UpArrow))) {
            if (mSelectedId > 0) {
                --mSelectedId;
            }
        }

        const std::vector<ConsoleLine> channel = mLog[mCurrentChannel];
        for (size_t i = 0; i < static_cast<int32_t>(channel.size()); i++) {
            ConsoleLine line = channel[i];
            if (!mFilter.empty() && line.Text.find(mFilter) == std::string::npos) {
                continue;
            }
            if (mLevelFilter > line.Priority) {
                continue;
            }
            std::string id = line.Text + "##" + std::to_string(i);
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            const bool isSelected = (mSelectedId == i) || std::find(mSelectedEntries.begin(), mSelectedEntries.end(),
                                                                    i) != mSelectedEntries.end();
            ImGui::PushStyleColor(ImGuiCol_Text, mPriorityColours[line.Priority]);
            if (ImGui::Selectable(id.c_str(), isSelected)) {
                if (ImGui::IsKeyDown(ImGui::GetKeyIndex(ImGuiKey_LeftCtrl)) && !isSelected) {
                    mSelectedEntries.push_back(i);

                } else {
                    mSelectedEntries.clear();
                }
                mSelectedId = isSelected ? -1 : i;
            }
            ImGui::PopStyleColor();
            if (isSelected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndTable();
    }
    ImGui::PopStyleColor();
    if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
        ImGui::SetScrollHereY(1.0f);
    }
    ImGui::EndChild();

    if (mCurrentChannel == "Console") {
        // Renders input textfield
        constexpr ImGuiInputTextFlags flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackEdit |
                                              ImGuiInputTextFlags_CallbackCompletion |
                                              ImGuiInputTextFlags_CallbackHistory;
#ifdef __WIIU__
        ImGui::PushItemWidth(-53.0f * 2.0f);
#else
        ImGui::PushItemWidth(-53.0f);
#endif
        if (ImGui::InputTextWithHint("##CMDInput", ">", mInputBuffer, gMaxBufferSize, flags,
                                     &ConsoleWindow::CallbackStub, this)) {
            inputFocus = true;
            if (mInputBuffer[0] != '\0' && mInputBuffer[0] != ' ') {
                Dispatch(std::string(mInputBuffer));
            }
            memset(mInputBuffer, 0, gMaxBufferSize);
        }

        if (mCmdHint != "None") {
            if (ImGui::IsItemFocused()) {
                ImGui::SetNextWindowPos(ImVec2(pos.x, pos.y + size.y));
                ImGui::SameLine();
                ImGui::BeginTooltip();
                ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
                ImGui::TextUnformatted(mCmdHint.c_str());
                ImGui::PopTextWrapPos();
                ImGui::EndTooltip();
            }
        }

        ImGui::SameLine();
#ifdef __WIIU__
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - 50 * 2.0f);
#else
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - 50);
#endif
        if (ImGui::Button("Submit") && !inputFocus && mInputBuffer[0] != '\0' && mInputBuffer[0] != ' ') {
            Dispatch(std::string(mInputBuffer));
            memset(mInputBuffer, 0, gMaxBufferSize);
        }

        ImGui::SetItemDefaultFocus();
        if (inputFocus) {
            ImGui::SetKeyboardFocusHere(-1);
        }
        ImGui::PopItemWidth();
    }
    ImGui::End();
}

void ConsoleWindow::Dispatch(const std::string& line) {
    mCmdHint = "None";
    mHistory.push_back(line);
    SendInfoMessage("> " + line);
    auto console = Context::GetInstance()->GetConsole();
    const std::vector<std::string> cmdArgs = StringHelper::Split(line, " ");
    if (console->HasCommand(cmdArgs[0])) {
        const CommandEntry entry = console->GetCommand(cmdArgs[0]);
        std::string output = "";
        int32_t commandResult = console->Run(line, &output);

        if (commandResult != 0) {
            SendErrorMessage(StringHelper::Sprintf("[LUS] Command Failed with code %d", commandResult));
            if (!output.empty()) {
                SendErrorMessage(output);
            }
            SendErrorMessage("[LUS] Usage: " + cmdArgs[0] + " " + console->BuildUsage(entry));
        } else {
            if (!output.empty()) {
                SendInfoMessage(output);
            } else {
                if (line != "clear") {
                    SendInfoMessage(std::string("[LUS] Command Success!"));
                }
            }
        }

        return;
    }
    SendErrorMessage("[LUS] Command not found");
}

int ConsoleWindow::CallbackStub(ImGuiInputTextCallbackData* data) {
    const auto instance = static_cast<ConsoleWindow*>(data->UserData);
    const bool emptyHistory = instance->mHistory.empty();
    const int historyIndex = instance->mHistoryIndex;
    auto console = Context::GetInstance()->GetConsole();
    std::string history;

    switch (data->EventKey) {
        case ImGuiKey_Tab:
            instance->mAutoComplete.clear();
            for (auto& [cmd, entry] : console->GetCommands()) {
                if (cmd.find(std::string(data->Buf)) != std::string::npos) {
                    instance->mAutoComplete.push_back(cmd);
                }
            }
            instance->mOpenAutocomplete = !instance->mAutoComplete.empty();
            instance->mCmdHint = "None";
            break;
        case ImGuiKey_UpArrow:
            if (emptyHistory) {
                break;
            }
            if (historyIndex < static_cast<int>(instance->mHistory.size()) - 1) {
                instance->mHistoryIndex += 1;
            }
            data->DeleteChars(0, data->BufTextLen);
            data->InsertChars(0, instance->mHistory[instance->mHistoryIndex].c_str());
            instance->mCmdHint = "None";
            break;
        case ImGuiKey_DownArrow:
            if (emptyHistory) {
                break;
            }
            if (historyIndex > -1) {
                instance->mHistoryIndex -= 1;
            }
            data->DeleteChars(0, data->BufTextLen);
            if (historyIndex >= 0) {
                data->InsertChars(0, instance->mHistory[historyIndex].c_str());
            }
            instance->mCmdHint = "None";
            break;
        case ImGuiKey_Escape:
            instance->mHistoryIndex = -1;
            data->DeleteChars(0, data->BufTextLen);
            instance->mOpenAutocomplete = false;
            instance->mCmdHint = "None";
            break;
        default:
            instance->mOpenAutocomplete = false;
            for (auto& [cmd, entry] : console->GetCommands()) {
                const std::vector<std::string> cmdArgs = StringHelper::Split(std::string(data->Buf), " ");
                if (data->BufTextLen > 2 && !cmdArgs.empty() && cmd.find(cmdArgs[0]) != std::string::npos) {
                    instance->mCmdHint = cmd + " " + console->BuildUsage(entry);
                    break;
                }
                instance->mCmdHint = "None";
            }
    }
    return 0;
}

void ConsoleWindow::Append(const std::string& channel, spdlog::level::level_enum priority, const char* fmt,
                           va_list args) {
    char buf[2048];
    vsnprintf(buf, IM_ARRAYSIZE(buf), fmt, args);
    buf[IM_ARRAYSIZE(buf) - 1] = 0;
    mLog[channel].push_back({ std::string(buf), priority });
}

void ConsoleWindow::Append(const std::string& channel, spdlog::level::level_enum priority, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    Append(channel, priority, fmt, args);
    va_end(args);
}

void ConsoleWindow::SendInfoMessage(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    Append("Console", spdlog::level::info, fmt, args);
    va_end(args);
}

void ConsoleWindow::SendErrorMessage(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    Append("Console", spdlog::level::err, fmt, args);
    va_end(args);
}

void ConsoleWindow::SendInfoMessage(const std::string& str) {
    Append("Console", spdlog::level::info, str.c_str());
}

void ConsoleWindow::SendErrorMessage(const std::string& str) {
    Append("Console", spdlog::level::err, str.c_str());
}

void ConsoleWindow::ClearLogs(std::string channel) {
    mLog[channel].clear();
}

void ConsoleWindow::ClearLogs() {
    for (auto [key, var] : mLog) {
        var.clear();
    }
}

std::string ConsoleWindow::GetCurrentChannel() {
    return mCurrentChannel;
}
} // namespace LUS
