#include "ResourceManager.h"
#include <spdlog/spdlog.h>
#include "File.h"
#include "Archive.h"
#include <algorithm>
#include <thread>
#include <Utils/StringHelper.h>
#include "public/bridge/consolevariablebridge.h"
#include "Context.h"

// Comes from stormlib. May not be the most efficient, but it's also important to be consistent.
// NOLINTNEXTLINE
extern bool SFileCheckWildCard(const char* szString, const char* szWildCard);

namespace LUS {

ResourceManager::ResourceManager(const std::string& mainPath, const std::string& patchesPath,
                                 const std::unordered_set<uint32_t>& validHashes, int32_t reservedThreadCount) {
    mResourceLoader = std::make_shared<ResourceLoader>();
    mArchive = std::make_shared<Archive>(mainPath, patchesPath, validHashes, false);
#if defined(__SWITCH__) || defined(__WIIU__)
    size_t threadCount = 1;
#else
    // the extra `- 1` is because we reserve an extra thread for spdlog
    size_t threadCount = std::max(1, (int32_t)(std::thread::hardware_concurrency() - reservedThreadCount - 1));
#endif
    mThreadPool = std::make_shared<BS::thread_pool>(threadCount);

    if (!DidLoadSuccessfully()) {
        // Nothing ever unpauses the thread pool since nothing will ever try to load the archive again.
        mThreadPool->pause();
    }
}

ResourceManager::ResourceManager(const std::vector<std::string>& otrFiles,
                                 const std::unordered_set<uint32_t>& validHashes, int32_t reservedThreadCount) {
    mResourceLoader = std::make_shared<ResourceLoader>();
    mArchive = std::make_shared<Archive>(otrFiles, validHashes, false);
#if defined(__SWITCH__) || defined(__WIIU__)
    size_t threadCount = 1;
#else
    // the extra `- 1` is because we reserve an extra thread for spdlog
    size_t threadCount = std::max(1, (int32_t)(std::thread::hardware_concurrency() - reservedThreadCount - 1));
#endif
    mThreadPool = std::make_shared<BS::thread_pool>(threadCount);

    if (!DidLoadSuccessfully()) {
        // Nothing ever unpauses the thread pool since nothing will ever try to load the archive again.
        mThreadPool->pause();
    }
}

ResourceManager::~ResourceManager() {
    SPDLOG_INFO("destruct ResourceManager");
}

bool ResourceManager::DidLoadSuccessfully() {
    return mArchive != nullptr && mArchive->IsMainMPQValid();
}

std::shared_ptr<File> ResourceManager::LoadFileProcess(const std::string& filePath) {
    auto file = mArchive->LoadFile(filePath, true);
    if (file != nullptr) {
        SPDLOG_TRACE("Loaded File {} on ResourceManager", file->Path);
    } else {
        SPDLOG_TRACE("Could not load File {} in ResourceManager", filePath);
    }
    return file;
}

std::shared_ptr<IResource> ResourceManager::LoadResourceProcess(const std::string& filePath, bool loadExact) {
    // Check for and remove the OTR signature
    if (OtrSignatureCheck(filePath.c_str())) {
        const auto newFilePath = filePath.substr(7);
        return LoadResourceProcess(newFilePath);
    }

    // Attempt to load the alternate version of the asset, if we fail then we continue trying to load the standard
    // asset.
    if (!loadExact && CVarGetInteger("gAltAssets", 0) && !filePath.starts_with(IResource::gAltAssetPrefix)) {
        const auto altPath = IResource::gAltAssetPrefix + filePath;
        auto altResource = LoadResourceProcess(altPath, loadExact);

        if (altResource != nullptr) {
            return altResource;
        }
    }

    // While waiting in the queue, another thread could have loaded the resource.
    // In a last attempt to avoid doing work that will be discarded, let's check if the cached version exists.
    auto cacheLine = CheckCache(filePath, loadExact);
    auto cachedResource = GetCachedResource(cacheLine);
    if (cachedResource != nullptr) {
        return cachedResource;
    }

    // Check for resource load errors which can indicate an alternate asset.
    // If we are attempting to load an alternate asset, we can return null
    if (!loadExact && CVarGetInteger("gAltAssets", 0) && filePath.starts_with(IResource::gAltAssetPrefix)) {
        if (std::holds_alternative<ResourceLoadError>(cacheLine)) {
            try {
                // If we have attempted to cache an alternate asset, but failed, we return nullptr and rely on the
                // calling function to return a regular asset. If we have NOT attempted load already, attempt the load.
                auto loadError = std::get<ResourceLoadError>(cacheLine);
                if (loadError != ResourceLoadError::NotCached) {
                    return nullptr;
                }
            } catch (std::bad_variant_access const& e) {
                // Ignore the exception. This should never happen. The last check should've returned the resource.
            }
        }
    }

    // Get the file from the OTR
    auto file = LoadFileProcess(filePath);
    if (file == nullptr) {
        SPDLOG_TRACE("Failed to load resource file at path {}", filePath);
    }

    // Transform the raw data into a resource
    auto resource = GetResourceLoader()->LoadResource(file);

    // Another thread could have loaded the resource while we were processing, so we want to check before setting to
    // the cache.
    cachedResource = GetCachedResource(filePath, true);
    {
        const std::lock_guard<std::mutex> lock(mMutex);

        if (cachedResource != nullptr) {
            // If another thread has already loaded this resource, discard the work we already did and return from
            // cache.
            resource = cachedResource;
        }

        // Set the cache to the loaded resource
        if (resource != nullptr) {
            mResourceCache[filePath] = resource;
        } else {
            mResourceCache[filePath] = ResourceLoadError::NotFound;
        }
    }

    if (resource != nullptr) {
        SPDLOG_TRACE("Loaded Resource {} on ResourceManager", filePath);
    } else {
        SPDLOG_TRACE("Resource load FAILED {} on ResourceManager", filePath);
    }

    return resource;
}

std::shared_future<std::shared_ptr<File>> ResourceManager::LoadFileAsync(const std::string& filePath, bool priority) {
    if (priority) {
        return mThreadPool->submit_front(&ResourceManager::LoadFileProcess, this, filePath).share();
    } else {
        return mThreadPool->submit_back(&ResourceManager::LoadFileProcess, this, filePath).share();
    }
}

std::shared_ptr<File> ResourceManager::LoadFile(const std::string& filePath) {
    return LoadFileAsync(filePath, true).get();
}

std::shared_future<std::shared_ptr<IResource>> ResourceManager::LoadResourceAsync(const std::string& filePath,
                                                                                  bool loadExact, bool priority) {
    // Check for and remove the OTR signature
    if (OtrSignatureCheck(filePath.c_str())) {
        auto newFilePath = filePath.substr(7);
        return LoadResourceAsync(newFilePath, loadExact, priority);
    }

    // Check the cache before queueing the job.
    auto cacheCheck = GetCachedResource(filePath, loadExact);
    if (cacheCheck) {
        auto promise = std::make_shared<std::promise<std::shared_ptr<IResource>>>();
        promise->set_value(cacheCheck);
        return promise->get_future().share();
    }

    const auto newFilePath = std::string(filePath);

    if (priority) {
        return mThreadPool->submit_front(&ResourceManager::LoadResourceProcess, this, newFilePath, loadExact);
    } else {
        return mThreadPool->submit_back(&ResourceManager::LoadResourceProcess, this, newFilePath, loadExact);
    }
}

std::shared_ptr<IResource> ResourceManager::LoadResource(const std::string& filePath, bool loadExact) {
    auto resource = LoadResourceAsync(filePath, loadExact, true).get();
    if (resource == nullptr) {
        SPDLOG_ERROR("Failed to load resource file at path {}", filePath);
    }
    return resource;
}

std::variant<ResourceManager::ResourceLoadError, std::shared_ptr<IResource>>
ResourceManager::CheckCache(const std::string& filePath, bool loadExact) {
    if (!loadExact && CVarGetInteger("gAltAssets", 0) && !filePath.starts_with(IResource::gAltAssetPrefix)) {
        const auto altPath = IResource::gAltAssetPrefix + filePath;
        auto altCacheResult = CheckCache(altPath, loadExact);

        // If the type held at this cache index is a resource, then we return it.
        // Else we attempt to load standard definition assets.
        if (std::holds_alternative<std::shared_ptr<IResource>>(altCacheResult)) {
            return altCacheResult;
        }
    }

    const std::lock_guard<std::mutex> lock(mMutex);

    auto resourceCacheFind = mResourceCache.find(filePath);
    if (resourceCacheFind == mResourceCache.end()) {
        return ResourceLoadError::NotCached;
    }

    return resourceCacheFind->second;
}

std::shared_ptr<IResource> ResourceManager::GetCachedResource(const std::string& filePath, bool loadExact) {
    // Gets the cached resource based on filePath.
    return GetCachedResource(CheckCache(filePath, loadExact));
}

std::shared_ptr<IResource>
ResourceManager::GetCachedResource(std::variant<ResourceLoadError, std::shared_ptr<IResource>> cacheLine) {
    // Gets the cached resource based on a cache line std::variant from the cache map.
    if (std::holds_alternative<std::shared_ptr<IResource>>(cacheLine)) {
        try {
            auto resource = std::get<std::shared_ptr<IResource>>(cacheLine);

            if (resource.use_count() <= 0) {
                return nullptr;
            }

            if (resource->IsDirty()) {
                return nullptr;
            }

            return resource;
        } catch (std::bad_variant_access const& e) {
            // Ignore the exception
        }
    }

    return nullptr;
}

std::shared_ptr<std::vector<std::shared_future<std::shared_ptr<IResource>>>>
ResourceManager::LoadDirectoryAsync(const std::string& searchMask, bool priority) {
    auto loadedList = std::make_shared<std::vector<std::shared_future<std::shared_ptr<IResource>>>>();
    auto fileList = GetArchive()->ListFiles(searchMask);
    loadedList->reserve(fileList->size());

    for (size_t i = 0; i < fileList->size(); i++) {
        auto fileName = std::string(fileList->operator[](i));
        auto future = LoadResourceAsync(fileName, false, priority);
        loadedList->push_back(future);
    }

    return loadedList;
}

std::shared_ptr<std::vector<std::shared_ptr<IResource>>> ResourceManager::LoadDirectory(const std::string& searchMask) {
    auto futureList = LoadDirectoryAsync(searchMask, true);
    auto loadedList = std::make_shared<std::vector<std::shared_ptr<IResource>>>();

    for (size_t i = 0; i < futureList->size(); i++) {
        const auto future = futureList->at(i);
        const auto resource = future.get();
        loadedList->push_back(resource);
    }

    return loadedList;
}

std::shared_ptr<std::vector<std::string>> ResourceManager::FindLoadedFiles(const std::string& searchMask) {
    const char* wildCard = searchMask.c_str();
    auto list = std::make_shared<std::vector<std::string>>();

    for (const auto& [key, value] : mResourceCache) {
        if (SFileCheckWildCard(key.c_str(), wildCard)) {
            list->push_back(key);
        }
    }

    return list;
}

void ResourceManager::DirtyDirectory(const std::string& searchMask) {
    auto list = FindLoadedFiles(searchMask);

    for (const auto& key : *list.get()) {
        auto resource = GetCachedResource(key);
        // If it's a resource, we will set the dirty flag, else we will just unload it.
        if (resource != nullptr) {
            resource->Dirty();
        } else {
            UnloadResource(key);
        }
    }
}

void ResourceManager::UnloadDirectory(const std::string& searchMask) {
    auto list = FindLoadedFiles(searchMask);

    for (const auto& key : *list.get()) {
        UnloadResource(key);
    }
}

std::shared_ptr<Archive> ResourceManager::GetArchive() {
    return mArchive;
}

std::shared_ptr<ResourceLoader> ResourceManager::GetResourceLoader() {
    return mResourceLoader;
}

size_t ResourceManager::UnloadResource(const std::string& filePath) {
    // Store a shared pointer here so that erase doesn't destruct the resource.
    // The resource will attempt to load other resources on the destructor, and this will fail because we already hold
    // the mutex.
    std::variant<ResourceLoadError, std::shared_ptr<IResource>> value = nullptr;
    size_t ret = 0;
    {
        const std::lock_guard<std::mutex> lock(mMutex);
        value = mResourceCache[filePath];
        ret = mResourceCache.erase(filePath);
    }

    return ret;
}

bool ResourceManager::OtrSignatureCheck(const char* fileName) {
    static const char* sOtrSignature = "__OTR__";
    return strncmp(fileName, sOtrSignature, strlen(sOtrSignature)) == 0;
}

} // namespace LUS
