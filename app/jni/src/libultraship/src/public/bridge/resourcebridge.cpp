#include "public/bridge/resourcebridge.h"
#include "Context.h"
#include <string>
#include <algorithm>
#include <StrHash64.h>

std::shared_ptr<LUS::IResource> ResourceLoad(const char* name) {
    return LUS::Context::GetInstance()->GetResourceManager()->LoadResource(name);
}

std::shared_ptr<LUS::IResource> ResourceLoad(uint64_t crc) {
    auto name = ResourceGetNameByCrc(crc);

    if (name == nullptr || strlen(name) == 0) {
        SPDLOG_TRACE("ResourceLoad: Unknown crc {}\n", crc);
        return nullptr;
    }

    return ResourceLoad(name);
}

extern "C" {

uint64_t ResourceGetCrcByName(const char* name) {
    return CRC64(name);
}

const char* ResourceGetNameByCrc(uint64_t crc) {
    const std::string* hashStr = LUS::Context::GetInstance()->GetResourceManager()->GetArchive()->HashToString(crc);
    return hashStr != nullptr ? hashStr->c_str() : nullptr;
}

size_t ResourceGetSizeByName(const char* name) {
    auto resource = ResourceLoad(name);

    if (resource == nullptr) {
        return 0;
    }

    return resource->GetPointerSize();
}

size_t ResourceGetSizeByCrc(uint64_t crc) {
    return ResourceGetSizeByName(ResourceGetNameByCrc(crc));
}

uint8_t ResourceGetIsCustomByName(const char* name) {
    auto resource = ResourceLoad(name);

    if (resource == nullptr) {
        return false;
    }

    return resource->GetInitData()->IsCustom;
}

uint8_t ResourceGetIsCustomByCrc(uint64_t crc) {
    return ResourceGetIsCustomByName(ResourceGetNameByCrc(crc));
}

void* ResourceGetDataByName(const char* name) {
    auto resource = ResourceLoad(name);

    if (resource == nullptr) {
        return nullptr;
    }

    return resource->GetRawPointer();
}

void* ResourceGetDataByCrc(uint64_t crc) {
    auto name = ResourceGetNameByCrc(crc);

    if (name == nullptr || strlen(name) == 0) {
        SPDLOG_TRACE("ResourceGetDataByCrc: Unknown crc {}\n", crc);
        return nullptr;
    }

    return ResourceGetDataByName(name);
}

uint16_t ResourceGetTexWidthByName(const char* name) {
    const auto res = static_pointer_cast<LUS::Texture>(ResourceLoad(name));

    if (res != nullptr) {
        return res->Width;
    }

    SPDLOG_ERROR("Given texture path is a non-existent resource");
    return -1;
}

uint16_t ResourceGetTexWidthByCrc(uint64_t crc) {
    const auto res = static_pointer_cast<LUS::Texture>(ResourceLoad(crc));

    if (res != nullptr) {
        return res->Width;
    }

    SPDLOG_ERROR("Given texture path is a non-existent resource");
    return -1;
}

uint16_t ResourceGetTexHeightByName(const char* name) {
    const auto res = static_pointer_cast<LUS::Texture>(ResourceLoad(name));

    if (res != nullptr) {
        return res->Height;
    }

    SPDLOG_ERROR("Given texture path is a non-existent resource");
    return -1;
}

uint16_t ResourceGetTexHeightByCrc(uint64_t crc) {
    const auto res = static_pointer_cast<LUS::Texture>(ResourceLoad(crc));

    if (res != nullptr) {
        return res->Height;
    }

    SPDLOG_ERROR("Given texture path is a non-existent resource");
    return -1;
}

size_t ResourceGetTexSizeByName(const char* name) {
    const auto res = static_pointer_cast<LUS::Texture>(ResourceLoad(name));

    if (res != nullptr) {
        return res->ImageDataSize;
    }

    SPDLOG_ERROR("Given texture path is a non-existent resource");
    return -1;
}

size_t ResourceGetTexSizeByCrc(uint64_t crc) {
    const auto res = static_pointer_cast<LUS::Texture>(ResourceLoad(crc));

    if (res != nullptr) {
        return res->ImageDataSize;
    }

    SPDLOG_ERROR("Given texture path is a non-existent resource");
    return -1;
}

void ResourceGetGameVersions(uint32_t* versions, size_t versionsSize, size_t* versionsCount) {
    auto list = LUS::Context::GetInstance()->GetResourceManager()->GetArchive()->GetGameVersions();
    memcpy(versions, list.data(), std::min(versionsSize, list.size() * sizeof(uint32_t)));
    *versionsCount = list.size();
}

void ResourceLoadDirectoryAsync(const char* name) {
    LUS::Context::GetInstance()->GetResourceManager()->LoadDirectoryAsync(name);
}

uint32_t ResourceHasGameVersion(uint32_t hash) {
    auto list = LUS::Context::GetInstance()->GetResourceManager()->GetArchive()->GetGameVersions();
    return std::find(list.begin(), list.end(), hash) != list.end();
}

void ResourceLoadDirectory(const char* name) {
    LUS::Context::GetInstance()->GetResourceManager()->LoadDirectory(name);
}

void ResourceDirtyDirectory(const char* name) {
    LUS::Context::GetInstance()->GetResourceManager()->DirtyDirectory(name);
}

void ResourceDirtyByName(const char* name) {
    auto resource = ResourceLoad(name);

    if (resource != nullptr) {
        resource->Dirty();
    }
}

void ResourceDirtyByCrc(uint64_t crc) {
    auto resource = ResourceLoad(crc);

    if (resource != nullptr) {
        resource->Dirty();
    }
}

void ResourceUnloadByName(const char* name) {
    LUS::Context::GetInstance()->GetResourceManager()->UnloadResource(name);
}

void ResourceUnloadByCrc(uint64_t crc) {
    ResourceUnloadByName(ResourceGetNameByCrc(crc));
}

void ResourceUnloadDirectory(const char* name) {
    LUS::Context::GetInstance()->GetResourceManager()->UnloadDirectory(name);
}

uint32_t ResourceDoesOtrFileExist() {
    return LUS::Context::GetInstance()->GetResourceManager()->DidLoadSuccessfully();
}
}
