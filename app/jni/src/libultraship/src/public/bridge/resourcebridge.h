#pragma once

#ifndef RESOURCEBRIDGE_H
#define RESOURCEBRIDGE_H

#include "stdint.h"

#ifdef __cplusplus
#include "resource/Archive.h"
#include "resource/type/Texture.h"
#include "resource/Resource.h"

std::shared_ptr<LUS::IResource> ResourceLoad(const char* name);
std::shared_ptr<LUS::IResource> ResourceLoad(uint64_t crc);
template <class T> std::shared_ptr<T> ResourceLoad(const char* name) {
    return static_pointer_cast<T>(ResourceLoad(name));
}
template <class T> std::shared_ptr<T> ResourceLoad(uint64_t crc) {
    return static_pointer_cast<T>(ResourceLoad(crc));
}

extern "C" {
#endif

uint64_t ResourceGetCrcByName(const char* name);
const char* ResourceGetNameByCrc(uint64_t crc);
size_t ResourceGetSizeByName(const char* name);
size_t ResourceGetSizeByCrc(uint64_t crc);
uint8_t ResourceGetIsCustomByName(const char* name);
uint8_t ResourceGetIsCustomByCrc(uint64_t crc);
void* ResourceGetDataByName(const char* name);
void* ResourceGetDataByCrc(uint64_t crc);
uint16_t ResourceGetTexWidthByName(const char* name);
uint16_t ResourceGetTexWidthByCrc(uint64_t crc);
uint16_t ResourceGetTexHeightByName(const char* name);
uint16_t ResourceGetTexHeightByCrc(uint64_t crc);
size_t ResourceGetTexSizeByName(const char* name);
size_t ResourceGetTexSizeByCrc(uint64_t crc);
void ResourceLoadDirectory(const char* name);
void ResourceLoadDirectoryAsync(const char* name);
void ResourceDirtyDirectory(const char* name);
void ResourceDirtyByName(const char* name);
void ResourceDirtyByCrc(uint64_t crc);
void ResourceUnloadByName(const char* name);
void ResourceUnloadByCrc(uint64_t crc);
void ResourceUnloadDirectory(const char* name);
void ResourceClearCache(void);
void ResourceGetGameVersions(uint32_t* versions, size_t versionsSize, size_t* versionsCount);
uint32_t ResourceHasGameVersion(uint32_t hash);
uint32_t ResourceDoesOtrFileExist();

#ifdef __cplusplus
};
#endif

#endif
