#pragma once

#include <string>
#include <memory>
#include <unordered_set>
#include <vector>
#include <spdlog/spdlog.h>
#include "config/Config.h"
#include "resource/ResourceManager.h"
#include "controller/ControlDeck.h"
#include "debug/CrashHandler.h"
#include "audio/Audio.h"
#include "window/Window.h"
#include "config/ConsoleVariable.h"
#include "debug/Console.h"

namespace LUS {

class Context {
  public:
    static std::shared_ptr<Context> GetInstance();
    static std::shared_ptr<Context> CreateInstance(const std::string name, const std::string shortName,
                                                   const std::string configFilePath,
                                                   const std::vector<std::string>& otrFiles = {},
                                                   const std::unordered_set<uint32_t>& validHashes = {},
                                                   uint32_t reservedThreadCount = 1);

    static std::string GetAppBundlePath();
    static std::string GetAppDirectoryPath(std::string appName = "");
    static std::string GetPathRelativeToAppDirectory(const std::string path, std::string appName = "");
    static std::string GetPathRelativeToAppBundle(const std::string path);
    static std::string LocateFileAcrossAppDirs(const std::string path, std::string appName = "");

    Context(std::string name, std::string shortName, std::string configFilePath);
    ~Context();

    void Init(const std::vector<std::string>& otrFiles, const std::unordered_set<uint32_t>& validHashes,
              uint32_t reservedThreadCount);

    std::shared_ptr<spdlog::logger> GetLogger();
    std::shared_ptr<Config> GetConfig();
    std::shared_ptr<ConsoleVariable> GetConsoleVariables();
    std::shared_ptr<ResourceManager> GetResourceManager();
    std::shared_ptr<ControlDeck> GetControlDeck();
    std::shared_ptr<CrashHandler> GetCrashHandler();
    std::shared_ptr<Window> GetWindow();
    std::shared_ptr<Console> GetConsole();
    std::shared_ptr<Audio> GetAudio();

    std::string GetConfigFilePath();
    std::string GetName();
    std::string GetShortName();

    void InitLogging();
    void InitConfiguration();
    void InitConsoleVariables();
    void InitResourceManager(const std::vector<std::string>& otrFiles = {},
                             const std::unordered_set<uint32_t>& validHashes = {}, uint32_t reservedThreadCount = 1);
    void InitControlDeck();
    void InitCrashHandler();
    void InitAudio();
    void InitConsole();
    void InitWindow();

  protected:
    Context() = default;

  private:
    static std::weak_ptr<Context> mContext;

    std::shared_ptr<spdlog::logger> mLogger;
    std::shared_ptr<Config> mConfig;
    std::shared_ptr<ConsoleVariable> mConsoleVariables;
    std::shared_ptr<ResourceManager> mResourceManager;
    std::shared_ptr<ControlDeck> mControlDeck;
    std::shared_ptr<CrashHandler> mCrashHandler;
    std::shared_ptr<Window> mWindow;
    std::shared_ptr<Console> mConsole;
    std::shared_ptr<Audio> mAudio;

    std::string mConfigFilePath;
    std::string mMainPath;
    std::string mPatchesPath;

    std::string mName;
    std::string mShortName;
};
} // namespace LUS
