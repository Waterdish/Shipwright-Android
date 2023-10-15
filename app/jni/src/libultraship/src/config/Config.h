#pragma once

#include <vector>
#include <string>
#include <nlohmann/json.hpp>

#include "audio/Audio.h"
#include "window/Window.h"

namespace LUS {

/**
 * @brief Abstract class representing a Config Version Updater, intended to express how to
 * upgrade a Configuration file from one version of a config to another (i.e. removing
 * default values, changing option names, etc.) It can be used by subclassing `ConfigVersionUpdater`,
 * implementing the Update function, and implementing the Constructor passing the version that the
 * Config is being updated to to this class' constructor from the child class' default constructor.
 * For example: \code ConfigVersion1Updater() : ConfigVersionUpdater(1) {} \endcode
 * Finally, give an instance of this subclass to a Config object via
 * RegisterConfigVersionUpdater and call RunVersionUpdates.
 */
class ConfigVersionUpdater {
  protected:
    uint32_t mVersion;

  public:
    ConfigVersionUpdater(uint32_t toVersion);
    /**
     * @brief Performs actions on a Config object via the provided pointer to update it
     * to the next version. (i.e. removing/changing default values or renaming options)
     *
     * @param conf
     */
    virtual void Update(Config* conf) = 0;

    /**
     * @brief Get the value of mVersion
     *
     * @return uint32_t
     */
    uint32_t GetVersion();
};
class Config {
  public:
    Config(std::string path);
    ~Config();

    std::string GetString(const std::string& key, const std::string& defaultValue = "");
    float GetFloat(const std::string& key, float defaultValue = 0.0f);
    bool GetBool(const std::string& key, bool defaultValue = false);
    int32_t GetInt(const std::string& key, int32_t defaultValue = 0);
    uint32_t GetUInt(const std::string& key, uint32_t defaultValue = 0);
    void SetString(const std::string& key, const std::string& value);
    void SetFloat(const std::string& key, float value);
    void SetBool(const std::string& key, bool value);
    void SetInt(const std::string& key, int32_t value);
    void SetUInt(const std::string& key, uint32_t value);
    void Erase(const std::string& key);
    bool Contains(const std::string& key);
    void Reload();
    void Save();
    nlohmann::json GetNestedJson();
    nlohmann::json GetFlattenedJson();
    bool IsNewInstance();

    AudioBackend GetAudioBackend();
    void SetAudioBackend(AudioBackend backend);
    WindowBackend GetWindowBackend();
    void SetWindowBackend(WindowBackend backend);

    /**
     * @brief Adds a ConfigVersionUpdater instance to the list to be run later via RunVersionUpdates
     *
     * @param versionUpdater
     * @return true if the insert was successful, or
     * @return false if the insert failed, i.e. if the list already has a ConfigVersionUpdater with
     * a matching version.
     */
    bool RegisterConfigVersionUpdater(std::shared_ptr<ConfigVersionUpdater> versionUpdater);

    /**
     * @brief Runs the Update function on each ConfigVersionUpdater instance if the version matches\
     * the current ConfigVersion value of the config object.
     *
     */
    void RunVersionUpdates();

  protected:
    nlohmann::json Nested(const std::string& key);
    static std::string FormatNestedKey(const std::string& key);
    template <typename T> void SetArray(const std::string& key, std::vector<T> array);
    template <typename T> std::vector<T> GetArray(const std::string& key);

  private:
    nlohmann::json mFlattenedJson;
    nlohmann::json mNestedJson;
    std::string mPath;
    bool mIsNewInstance;
    std::map<uint32_t, std::shared_ptr<ConfigVersionUpdater>> mVersionUpdaters;
};
} // namespace LUS
