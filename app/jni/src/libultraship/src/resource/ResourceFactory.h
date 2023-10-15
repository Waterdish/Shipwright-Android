#pragma once

#include <memory>
#include "utils/binarytools/BinaryReader.h"
#include <tinyxml2.h>
#include "Resource.h"

namespace LUS {
class ResourceManager;
class ResourceFactory {
  public:
    virtual std::shared_ptr<IResource> ReadResource(std::shared_ptr<ResourceInitData> initData,
                                                    std::shared_ptr<BinaryReader> reader) = 0;
    virtual std::shared_ptr<IResource> ReadResourceXML(std::shared_ptr<ResourceInitData> initData,
                                                       tinyxml2::XMLElement* reader);
};

class ResourceVersionFactory {
  public:
    virtual void ParseFileBinary(std::shared_ptr<BinaryReader> reader, std::shared_ptr<IResource> resource);
    virtual void ParseFileXML(tinyxml2::XMLElement* reader, std::shared_ptr<IResource> resource);
    virtual void WriteFileBinary(std::shared_ptr<BinaryWriter> writer, std::shared_ptr<IResource> resource);
    virtual void WriteFileXML(tinyxml2::XMLElement* writer, std::shared_ptr<IResource> resource);
};
} // namespace LUS
