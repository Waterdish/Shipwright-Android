#pragma once

#include <stdint.h>
#include <string>
#include <memory>
#include <vector>
#include <functional>
#include <map>
#include <ImGui/imgui.h>

namespace LUS {

class Console;
typedef std::function<int32_t(std::shared_ptr<Console> console, std::vector<std::string> args, std::string* output)>
    CommandHandler;

enum class ArgumentType { TEXT, NUMBER };

struct CommandArgument {
    std::string Info;
    ArgumentType Type = ArgumentType::NUMBER;
    bool Optional = false;
};

struct CommandEntry {
    CommandHandler Handler;
    std::string Description;
    std::vector<CommandArgument> Arguments;
};

class Console {
  public:
    Console();
    ~Console();

    void Init();
    int32_t Run(const std::string& command, std::string* output);
    bool HasCommand(const std::string& command);
    void AddCommand(const std::string& command, CommandEntry entry);
    std::string BuildUsage(const std::string& command);
    std::string BuildUsage(const CommandEntry& entry);
    CommandEntry& GetCommand(const std::string& command);
    std::map<std::string, CommandEntry>& GetCommands();

  protected:
  private:
    std::map<std::string, CommandEntry> mCommands;
};

} // namespace LUS
