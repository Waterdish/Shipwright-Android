#pragma once

#include <stdint.h>
#include "ResourceType.h"
#include "utils/binarytools/BinaryWriter.h"

namespace LUS {
class ResourceManager;

struct ResourceInitData {
    std::string Path;
    Endianness ByteOrder;
    ResourceType Type;
    int32_t ResourceVersion;
    uint64_t Id;
    bool IsCustom;
};

class IResource {
  public:
    inline static const std::string gAltAssetPrefix = "alt/";

    IResource(std::shared_ptr<ResourceInitData> initData);
    virtual ~IResource();

    virtual void* GetRawPointer() = 0;
    virtual size_t GetPointerSize() = 0;

    bool IsDirty();
    void Dirty();
    std::shared_ptr<ResourceInitData> GetInitData();

  private:
    std::shared_ptr<ResourceInitData> mInitData;
    bool mIsDirty = false;
};

template <class T> class Resource : public IResource {
  public:
    using IResource::IResource;
    virtual T* GetPointer() = 0;
    void* GetRawPointer() override {
        return static_cast<void*>(GetPointer());
    }
};

} // namespace LUS
