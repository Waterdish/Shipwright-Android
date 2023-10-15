#include "Console.h"
#include "Utils/StringHelper.h"
#include "Context.h"

namespace LUS {
Console::Console() {
}

Console::~Console() {
    SPDLOG_TRACE("destruct console");
}

void Console::Init() {
}

std::string Console::BuildUsage(const CommandEntry& entry) {
    std::string usage;
    for (const auto& arg : entry.Arguments) {
        usage += StringHelper::Sprintf(arg.Optional ? "[%s] " : "<%s> ", arg.Info.c_str());
    }
    return usage;
}

std::string Console::BuildUsage(const std::string& command) {
    return BuildUsage(GetCommand(command));
}

int32_t Console::Run(const std::string& command, std::string* output) {
    const std::vector<std::string> cmdArgs = StringHelper::Split(command, " ");
    if (cmdArgs.empty()) {
        SPDLOG_INFO("Could not parse command: {}", command);
        return false;
    }

    const std::string& commandName = cmdArgs[0];
    if (!mCommands.contains(commandName)) {
        SPDLOG_INFO("Command handler not found: {}", commandName);
        return false;
    }

    const CommandEntry entry = mCommands[commandName];
    int32_t commandResult = entry.Handler(Context::GetInstance()->GetConsole(), cmdArgs, output);
    if (output) {
        SPDLOG_INFO("Command \"{}\" returned {} with output: {}", command, commandResult, *output);
    } else {
        SPDLOG_INFO("Command \"{}\" returned {}", command, commandResult);
    }
    return commandResult;
}

bool Console::HasCommand(const std::string& command) {
    for (const auto& value : mCommands) {
        if (value.first == command) {
            return true;
        }
    }

    return false;
}

void Console::AddCommand(const std::string& command, CommandEntry entry) {
    if (!HasCommand(command)) {
        mCommands[command] = entry;
    } else {
        SPDLOG_WARN("Attempting to add command {} that already exists", command);
    }
}

std::map<std::string, CommandEntry>& Console::GetCommands() {
    return mCommands;
}

CommandEntry& Console::GetCommand(const std::string& command) {
    return mCommands[command];
}
} // namespace LUS
