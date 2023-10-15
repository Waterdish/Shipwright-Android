#pragma once

#include <unordered_map>
#include <string>
#include <mutex>
#include <queue>
#include <variant>
#include "Resource.h"
#include "ResourceLoader.h"
#include "Archive.h"
#include "thread-pool/BS_thread_pool.hpp"

namespace LUS {
struct File;

// Resource manager caches any and all files it comes across into memory. This will be unoptimal in the future when
// modifications have gigabytes of assets. It works with the original game's assets because the entire ROM is 64MB and
// fits into RAM of any semi-modern PC.
class ResourceManager {
    typedef enum class ResourceLoadError { None, NotCached, NotFound } ResourceLoadError;

  public:
    ResourceManager(const std::string& mainPath, const std::string& patchesPath,
                    const std::unordered_set<uint32_t>& validHashes, int32_t reservedThreadCount = 1);
    ResourceManager(const std::vector<std::string>& otrFiles, const std::unordered_set<uint32_t>& validHashes,
                    int32_t reservedThreadCount = 1);
    ~ResourceManager();

    bool DidLoadSuccessfully();
    std::shared_ptr<Archive> GetArchive();
    std::shared_ptr<ResourceLoader> GetResourceLoader();
    std::shared_future<std::shared_ptr<File>> LoadFileAsync(const std::string& filePath, bool priority = false);
    std::shared_ptr<File> LoadFile(const std::string& filePath);
    std::shared_ptr<IResource> GetCachedResource(const std::string& filePath, bool loadExact = false);
    std::shared_ptr<IResource> LoadResource(const std::string& filePath, bool loadExact = false);
    std::shared_ptr<IResource> LoadResourceProcess(const std::string& filePath, bool loadExact = false);
    size_t UnloadResource(const std::string& filePath);
    std::shared_future<std::shared_ptr<IResource>> LoadResourceAsync(const std::string& filePath,
                                                                     bool loadExact = false, bool priority = false);
    std::shared_ptr<std::vector<std::shared_ptr<IResource>>> LoadDirectory(const std::string& searchMask);
    std::shared_ptr<std::vector<std::shared_future<std::shared_ptr<IResource>>>>
    LoadDirectoryAsync(const std::string& searchMask, bool priority = false);
    std::shared_ptr<std::vector<std::string>> FindLoadedFiles(const std::string& searchMask);
    void DirtyDirectory(const std::string& searchMask);
    void UnloadDirectory(const std::string& searchMask);
    bool OtrSignatureCheck(const char* fileName);

  protected:
    std::shared_ptr<File> LoadFileProcess(const std::string& filePath);
    std::shared_ptr<IResource> GetCachedResource(std::variant<ResourceLoadError, std::shared_ptr<IResource>> cacheLine);
    std::variant<ResourceLoadError, std::shared_ptr<IResource>> CheckCache(const std::string& filePath,
                                                                           bool loadExact = false);

  private:
    std::unordered_map<std::string, std::variant<ResourceLoadError, std::shared_ptr<IResource>>> mResourceCache;
    std::shared_ptr<ResourceLoader> mResourceLoader;
    std::shared_ptr<Archive> mArchive;
    std::shared_ptr<BS::thread_pool> mThreadPool;
    std::mutex mMutex;
};
} // namespace LUS
